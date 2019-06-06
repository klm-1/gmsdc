#ifndef GRAPHMLWRITER_H
#define GRAPHMLWRITER_H

#include <iosfwd>
#include <string>

#include "flowgraph.h"
#include "indentablewriter.h"

class GmAST;
class ControlTree;

class GraphmlWriter : public IndentableWriter
{
public:
    GraphmlWriter(std::ostream& os);

    void print(const GmAST& ast);
    void print(const FlowGraph& g);
    void print(const ControlTree& t);

private:
    void print_impl(const GmAST& ast);
    void print_impl(const ControlTree& t);
    void print_node(const FlowGraph::Node& n);
    void print_outputs(const FlowGraph::Node& n);
    void enter(const std::string& t);
    void enterGraph();
    void enterNode(size_t id);
    void enterEdge(size_t src, size_t dest);
    void writeField(const std::string& key, const std::string& val);
    void writeFieldQuoted(const std::string& key, const std::string& val);
    void writeLabel(const std::string& val);
    void writeNodeLabelGraphics();
    void writeEdgeLabelGraphics();
    void writeEdgeLineStyleDashed();
    void leave();
};

#endif // GRAPHMLWRITER_H
