#include "../include/filesystem_node.h"
#include "../include/directory.h"

FileSystemNode::FileSystemNode(const std::string& name, Directory* parent)
    : name_(name), parent_(parent) {}

FileSystemNode::~FileSystemNode() = default;

const std::string& FileSystemNode::getName() const {
    return name_;
}

Directory* FileSystemNode::getParent() const {
    return parent_;
}

void FileSystemNode::rename(const std::string& newName) {
    name_ = newName;
}

void FileSystemNode::printIndent(int indent) const {
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
}