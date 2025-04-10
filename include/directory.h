#ifndef DIRECTORY_H
#define DIRECTORY_H

#include "filesystem_node.h"
#include <map>
#include <memory>
#include <vector>
#include <string>

class File;  // Forward declaration

class Directory : public FileSystemNode {
public:
    Directory(const std::string& name, Directory* parent);
    ~Directory() override;

    bool isDirectory() const override;
    void listContents(int indent = 0) const override;

    bool addChild(std::unique_ptr<FileSystemNode> child);
    bool removeChild(const std::string& name);
    FileSystemNode* getChild(const std::string& name) const;
    Directory* getSubDirectory(const std::string& name) const;
    File* getFile(const std::string& name) const;
    std::vector<std::string> getChildNames() const;

    std::unique_ptr<FileSystemNode> removeChildAndReturn(const std::string& name);
    void insertChild(const std::string& name, std::unique_ptr<FileSystemNode> child);

private:
    std::map<std::string, std::unique_ptr<FileSystemNode>> children_;
};

#endif // DIRECTORY_H