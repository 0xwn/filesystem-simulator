#ifndef FILE_H
#define FILE_H

#include "filesystem_node.h"
#include <string>

class File : public FileSystemNode {
public:
    File(const std::string& name, Directory* parent);

    bool isDirectory() const override;
    void listContents(int indent = 0) const override;

    void setContent(const std::string& content);
    const std::string& getContent() const;

private:
    std::string content_;
};

#endif // FILE_H