#include "graphmlwriter.h"

#include <iostream>

#include "gmast.h"
#include "controltree.h"

using namespace std;


GraphmlWriter::GraphmlWriter(std::ostream& os)
    : IndentableWriter(os)
{}

void GraphmlWriter::print(const GmAST& ast)
{
    enterGraph();
    print_impl(ast);
    leave();
}

void GraphmlWriter::print(const FlowGraph& g)
{
    enterGraph();
    for(const auto& n : g.nodes_) 
	{
        print_node(*n);
    }
    for(const auto& n : g.nodes_) 
	{
        print_outputs(*n);
    }
    leave();
}

void GraphmlWriter::print(const ControlTree& t)
{
    enterGraph();
    print_impl(t);
    leave();
}

void GraphmlWriter::print_impl(const GmAST& ast)
{
    size_t id = reinterpret_cast<uintptr_t>(&ast);

    enterNode(id);
    writeLabel(to_string(ast));
    leave();

    size_t labelId = 0;
    for(const auto& l : ast.links_) 
	{
        print_impl(*l);

        enterEdge(id, reinterpret_cast<uintptr_t>(l.get()));
        writeLabel(to_string(labelId++));
        writeEdgeLabelGraphics();
        leave();
    }
}

void GraphmlWriter::print_impl(const ControlTree& t)
{
    size_t id = reinterpret_cast<uintptr_t>(&t);

    enterNode(id);
    if(t.type() == ControlTree::Type::Terminal) 
	{
        writeLabel(to_string(t));
    } 
	else 
	{
        writeLabel(ControlTreeTypeToString(t.type()));
    }
    writeNodeLabelGraphics();
    leave();

    size_t labelId = 0;
    for(const auto& ptr : t.leaves_) 
	{
        print_impl(*ptr);

        enterEdge(id, reinterpret_cast<uintptr_t>(ptr.get()));
        writeLabel(to_string(labelId++));
        writeEdgeLabelGraphics();
        leave();
    }
}

void GraphmlWriter::print_node(const FlowGraph::Node& n)
{
    enterNode(reinterpret_cast<uintptr_t>(&n));
    writeLabel(to_string(*n.tree));
    writeNodeLabelGraphics();
    leave();
}

void GraphmlWriter::print_outputs(const FlowGraph::Node& n)
{
    bool first = true;
    for(const auto ptr : n.outputs) 
	{
        enterEdge(reinterpret_cast<uintptr_t>(&n), reinterpret_cast<uintptr_t>(ptr));
        if(!first) 
		{
            writeEdgeLineStyleDashed();
        }
        first = false;
        leave();
    }
}

void GraphmlWriter::enter(const string& t)
{
    out() << indent() << t << " [\n";
    stepIn();
}

void GraphmlWriter::leave()
{
    stepOut();
    out() << indent() << "]\n";
}

void GraphmlWriter::enterGraph()
{
    writeFieldQuoted("Creator", "GMSDC");
    enter("graph");
    writeField("directed", "1");
}

void GraphmlWriter::enterNode(size_t id)
{
    enter("node");
    writeField("id", to_string(id));
}

void GraphmlWriter::enterEdge(size_t src, size_t dest)
{
    enter("edge");
    writeField("source", to_string(src));
    writeField("target", to_string(dest));
}

void GraphmlWriter::writeField(const string& key, const string& val)
{
    out() << indent() << key << " " << val << "\n";
}

void GraphmlWriter::writeFieldQuoted(const string& key, const string& val)
{
    string esc(val);
    string_replace_char(esc, '&', "&amp;");
    string_replace_char(esc, '"', "&quot;");
    out() << indent() << key << " \"" << esc << "\"\n";
}

void GraphmlWriter::writeLabel(const string& val)
{
    writeFieldQuoted("label", val);
}

void GraphmlWriter::writeNodeLabelGraphics()
{
    enter("LabelGraphics");
    writeFieldQuoted("alignment", "left");
    leave();
}

void GraphmlWriter::writeEdgeLabelGraphics()
{
    enter("LabelGraphics");
    writeFieldQuoted("model", "six_pos");
    writeFieldQuoted("position", "ttail");
    leave();
}

void GraphmlWriter::writeEdgeLineStyleDashed()
{
    enter("graphics");
    writeFieldQuoted("style", "dashed");
    writeFieldQuoted("targetArrow", "standard");
    leave();
}
