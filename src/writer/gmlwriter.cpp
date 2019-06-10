#include "gmlwriter.h"

#include <iostream>
#include <iomanip>
#include <chrono>
#include <ctime>

#include "gmast.h"
#include "utils.h"

using namespace std;
using namespace chrono;
using namespace string_literals;


GmlWriter::GmlWriter(std::ostream& os, GmForm& f)
    : IndentableWriter(os)
    , form_(f)
    , locals_()
{}

void GmlWriter::print(const GmAST& ast)
{
    out() <<
          "/*** ********************* ***\n"
          " *** Decompiled with GMSDC ***\n"
          " *** ********************* ***/\n";

    writePaddingLine();
    writeLocalVariables(ast);
    writePaddingLine();

    writeCode(ast, false);
    writePaddingLine();
    writeDatetime();
}

void GmlWriter::writePaddingLine()
{
    if (padAllowed_)
	{
        padAllowed_ = false;
        out() << "\n";
    }
}

void GmlWriter::writeLocalVariables(const GmAST& ast)
{
    locals_.clear();
    collectLocalVariables(ast);

    if (locals_.empty())  { return; }

    beginLine("var ");
    copy(++locals_.begin(), locals_.end(), ostream_iterator<string>(out(), ", "));
    out() << *locals_.begin();
    endLine(";");
}

void GmlWriter::writeDatetime()
{
    time_t now = system_clock::to_time_t(system_clock::now());
    beginLine("/*** ");
    out() << put_time(gmtime(&now), "%Y-%m-%d %H:%M:%S");
    endLine(" ***/");
}

void GmlWriter::writeHexadecimal(unsigned x, unsigned prec)
{
    char fmt[8];
    sprintf(fmt, "%%0%uX", prec);
    char hex[16];
    sprintf(hex, fmt, x);
    out() << "$" << hex;
}

void GmlWriter::writeLine(const char* str)
{
    padAllowed_ = true;
    out() << indent() << str << "\n";
}

void GmlWriter::beginLine(const char* str)
{
    padAllowed_ = true;
    out() << indent() << str;
}

void GmlWriter::endLine(const char* str)
{
    out() << str << "\n";
}

void GmlWriter::collectLocalVariables(const GmAST& ast)
{
    const GmAST* scope = nullptr;
    switch (ast.pattern())
	{
        case (GmlPattern::Variable):
            scope = ast.leaf()->leavesCount() == 0 ? ast.leaf() : ast.leaf()->leaf();
            break;

        case (GmlPattern::ArrayElement):
        case (GmlPattern::ArrayElement2):
            scope = ast.rightLeaf()->leavesCount() == 0 ? ast.rightLeaf() : ast.rightLeaf()->leaf();
            break;

        default:
            break;
    }

    if (scope && scope->dataInt() == static_cast<int>(InstanceType::Local))
	{
        locals_.insert(ast.dataString());
    }

    for (const auto& l : ast.links_)
	{
        collectLocalVariables(*l);
    }
}

void GmlWriter::writeScope(const GmAST& ast)
{
    if (ast.leavesCount() == 0)
	{
        int64_t itype = ast.dataInt();
        InstanceType t = InstanceType(itype);

        if (itype < 0)
		{
            if (t != InstanceType::Local &&
                (t != InstanceType::Self || locals_.find(ast.uplink_->dataString()) != locals_.end()))
			{
                out() << InstanceType2PrettyString(t) << ".";
            }
        }
		else if (const char* obj = queryForm(itype, ExprContext::Object))
		{
            out() << obj << ".";
        }
		else
		{
            out() << "(" << itype << ").";
        }
    }
	else if (ast.leaf()->pattern() == GmlPattern::Number)
	{
        writeScope(*ast.leaf());
    }
	else
	{
        bool par = ast.leaf()->pattern() == GmlPattern::BinaryOp ||
                   ast.leaf()->pattern() == GmlPattern::UnaryOp ||
                   ast.leaf()->pattern() == GmlPattern::FunctionCall ||
                   ast.leaf()->pattern() == GmlPattern::Prefix;
        out() << (par ? "(" : "");
        writeExpression(*ast.leaf());
        out() << (par ? ")" : "");
        out() << ".";
    }
}

const char* GmlWriter::queryForm(int n, ExprContext::Type ctx)
{
    switch (ctx)
	{
        case (ExprContext::Sprite):
            return queryChunk(form_.sprites, n);

        case (ExprContext::Sound):
            return queryChunk(form_.sounds, n);

        case (ExprContext::Background):
            return queryChunk(form_.backgrounds, n);

        case (ExprContext::Path):
            return queryChunk(form_.paths, n);

        case (ExprContext::Script):
            return queryChunk(form_.scripts, n);

        case (ExprContext::Shader):
            return queryChunk(form_.shaders, n);

        case (ExprContext::Font):
            return queryChunk(form_.fonts, n);

        case (ExprContext::Timeline):
            return queryChunk(form_.timelines, n);

        case (ExprContext::Object):
            return queryChunk(form_.objects, n);

        case (ExprContext::Room):
            return queryChunk(form_.rooms, n);

        // case (ExprContext::File):
        // case (ExprContext::Extension):

        default:
            return nullptr;
    }
}

void GmlWriter::writeExpression(const GmAST& ast, ExprContext::Type ctx)
{
    int64_t n = ast.dataInt();

    if (ast.pattern() != GmlPattern::Number || to_string(n) != ast.dataString())
	{
        writeExpression(ast);
        return;
    }

    const char* val = ExprContext::ValueInContext(n, ctx);
    if (val)
	{
        out() << val;
        return;
    }

    if (ctx == ExprContext::Color)
	{
        writeHexadecimal(n, 6);
        return;
    }

    const char* name = queryForm(n, ctx);
    if (name)
	{
        out() << name;
        return;
    }

    if (ctx == ExprContext::Object && n < 0 && n >= -7)
	{
        out() << InstanceType2PrettyString(InstanceType(n));
        return;
    }

    out() << ast.dataString();
}

void GmlWriter::writeExpression(const GmAST& ast)
{
    switch (ast.pattern())
	{
        case (GmlPattern::BinaryOp):
            writeBinaryOp(ast);
            break;

        case (GmlPattern::UnaryOp):
			{
                bool par = ast.leaf()->pattern() == GmlPattern::BinaryOp;
                out() << ast.dataString();
                out() << (par ? "(" : "");
                writeExpression(*ast.leaf());
                out() << (par ? ")" : "");
            }
            break;

        case (GmlPattern::Number):
            out() << ast.dataString();
            break;

        case (GmlPattern::String):
            out() << '"' << ast.dataString() << '"';
            break;

        case (GmlPattern::FunctionCall):
            writeFunctionCall(ast);
            break;

        case (GmlPattern::Prefix):
            out() << ast.dataString();
            writeExpression(*ast.leaf());
            break;

        case (GmlPattern::Postfix):
            writeExpression(*ast.leaf());
            out() << ast.dataString();
            break;

        case (GmlPattern::Variable):
            writeScope(*ast.leaf());
            out() << ast.dataString();
            break;

        case (GmlPattern::ArrayElement):
            writeScope(*ast.rightLeaf());
            out() << ast.dataString() << "[";
            writeExpression(*ast.leftLeaf());
            out() << "]";
            break;

        case (GmlPattern::ArrayElement2):
            writeScope(*ast.rightLeaf());
            out() << ast.dataString() << "[";
            writeExpression(*ast.leftLeaf());
            out() << ", ";
            writeExpression(*ast.leaf(1));
            out() << "]";
            break;

        default:
            throw runtime_error(GmlPattern2String(ast.pattern()) + " is not an expression"s);
    }
}

void GmlWriter::writeBinaryOp(const GmAST& ast)
{
    const GmAST* l = ast.leftLeaf();
    const GmAST* r = ast.rightLeaf();
    string op = ast.dataString();
    string lop = l->dataString();
    string rop = r->dataString();

    bool leftp = l->pattern() == GmlPattern::BinaryOp && lookup(BinaryOpPriority, op) >= lookup(BinaryOpPriority, lop);
    bool rightp = r->pattern() == GmlPattern::BinaryOp && lookup(BinaryOpPriority, op) > lookup(BinaryOpPriority, rop);
    if (leftp && op == lop && (op == "||" || op == "&&"))
	{
        leftp = false;
    }

    out() << (rightp ? "(" : "");
    writeExpression(*r);
    out() << (rightp ? ")" : "");

    out() << " " << op << " ";

    out() << (leftp ? "(" : "");
    writeExpression(*l);
    out() << (leftp ? ")" : "");
}

void GmlWriter::writeFunctionCall(const GmAST& ast)
{
    const auto* ctx = ExprContext::FuncArgContext(ast.dataString());
    bool pad = ast.leavesCount() > 0;

    out() << ast.dataString().c_str() << "(";
    out() << (pad ? " " : "");

    int i = 0;
    for (const auto& l : ast.links_)
	{
        if (l != *ast.links_.begin())
		{
            out() << ", ";
        }
        if (ctx)
		{
            writeExpression(*l, ctx->at(i));
        }
		else
		{
            writeExpression(*l);
        }
        ++i;
    }

    out() << (pad ? " " : "");
    out() << ")";
}

void GmlWriter::writeCode(const GmAST& ast, bool ind)
{
    if (ind)
	{
        stepIn();
    }

    switch (ast.pattern())
	{
        case (GmlPattern::If):
            beginLine("if (");
            writeExpression(*ast.rightLeaf());
            endLine(") {");
            writeCode(*ast.leftLeaf());
            writeLine("}");
            break;

        case (GmlPattern::IfElse):
            beginLine("if (");
            writeExpression(*ast.rightLeaf());
            endLine(") {");
            writeCode(*ast.leaf(1));
            writeLine("} else {");
            writeCode(*ast.leftLeaf());
            writeLine("}");
            break;

        case (GmlPattern::While):
            beginLine("while (");
            writeExpression(*ast.rightLeaf());
            endLine(") {");
            writeCode(*ast.leftLeaf());
            writeLine("}");
            break;

        case (GmlPattern::Repeat):
            beginLine("repeat (");
            writeExpression(*ast.rightLeaf());
            endLine(") {");
            writeCode(*ast.leftLeaf());
            writeLine("}");
            break;

        case (GmlPattern::Break):
            writeLine("break;");
            break;

        case (GmlPattern::Continue):
            writeLine("continue;");
            break;

        case (GmlPattern::DoUntil):
            writeLine("do {");
            writeCode(*ast.leftLeaf());
            beginLine("} until(");
            writeExpression(*ast.rightLeaf());
            endLine(")");
            break;

        case (GmlPattern::LinearBlock):
            for (const auto& l : reversed(ast.links_))
			{
                bool first = l == *ast.links_.rbegin();
                bool last = l == *ast.links_.begin();
                bool p = GmlNeedsPadding(l->pattern());
                if (!first && p)
				{
                    writePaddingLine();
                }
                writeCode(*l, false);
                if (!last && p)
				{
                    writePaddingLine();
                }
            }
            break;

        case (GmlPattern::Assignment):
            beginLine();
            writeExpression(*ast.leftLeaf());
            out() << " = ";
            writeExpression(*ast.rightLeaf());
            endLine(";");
            break;

        case (GmlPattern::CompoundAssignment):
            beginLine();
            writeExpression(*ast.leftLeaf());
            out() << " " << ast.dataString() << " ";
            writeExpression(*ast.rightLeaf());
            endLine(";");
            break;

        case (GmlPattern::FunctionCall):
            beginLine();
            writeExpression(ast);
            endLine(";");
            break;

        case (GmlPattern::Return):
            beginLine("return ");
            writeExpression(*ast.leaf());
            endLine(";");
            break;

        case (GmlPattern::Exit):
            writeLine("exit;");
            break;

        case (GmlPattern::DroppedExpr):
            if (ast.leaf()->pattern() == GmlPattern::FunctionCall)
			{
                writeCode(*ast.leaf(), false);
            }
            break;

        case (GmlPattern::Nop):
            break;

        case (GmlPattern::Switch):
            beginLine("switch (");
            writeExpression(*ast.rightLeaf());
            endLine(") {");
            for (const auto& l : reversed(ast.links_))
			{
                if (l == *ast.links_.rbegin())
				{
                    continue;
                }
                if (l != *++ast.links_.rbegin())
				{
                    writePaddingLine();
                }
                writeCode(*l);
            }
            writeLine("}");
            break;

        case (GmlPattern::SwitchCase):
            beginLine("case (");
            writeExpression(*ast.rightLeaf());
            endLine("):");
            writeCode(*ast.leftLeaf());
            break;

        case (GmlPattern::SwitchDefault):
            writeLine("default:");
            writeCode(*ast.leaf());
            break;

        case (GmlPattern::With):
            beginLine("with(");
            writeExpression(*ast.rightLeaf(), ExprContext::Object);
            endLine(") {");
            writeCode(*ast.leftLeaf());
            writeLine("}");
            break;

        case (GmlPattern::Invalid):
            throw runtime_error("Pattern '"s + GmlPattern2String(ast.pattern()) + "' found in AST");
            break;

        default:
            throw runtime_error(GmlPattern2String(ast.pattern()) + " is not a statement"s);
    }

    if (ind) {
        stepOut();
    }
}
