#ifndef FILESYSTEM_NODE_H
#define FILESYSTEM_NODE_H

#include <string>
#include <iostream>

class Directory;  // Forward declaration

class FileSystemNode {
public:
    FileSystemNode(const std::string& name, Directory* parent);
    virtual ~FileSystemNode();

    const std::string& getName() const;
    Directory* getParent() const;

    virtual bool isDirectory() const = 0;
    virtual void listContents(int indent = 0) const = 0;

    void rename(const std::string& newName);

protected:
    std::string name_;
    Directory* parent_;

    void printIndent(int indent) const;
};

#endif // FILESYSTEM_NODE_H