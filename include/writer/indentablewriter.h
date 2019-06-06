#ifndef INDENTABLEWRITER_H
#define INDENTABLEWRITER_H

#include <iosfwd>


class IndentableWriter
{
public:
    IndentableWriter(std::ostream&);
    virtual ~IndentableWriter() = default;

protected:
    int depth = 0;

    void stepIn();
    void stepOut();
    std::ostream& out();
    const char* indent(int t);
    const char* indent();

private:
    std::ostream& out_;
    char spaces_[256];
    int prev_ = -1;
};

#endif // INDENTABLEWRITER_H
