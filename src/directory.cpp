#include "../include/directory.h"
#include "../include/file.h"

#include <iostream>
#include <algorithm>

Directory::Directory(const std::string& name, Directory* parent)
    : FileSystemNode(name, parent) {}

Directory::~Directory() = default;

bool Directory::isDirectory() const {
    return true;
}

void Directory::listContents(int indent) const {
    printIndent(indent);
    std::cout << "+ " << getName() << " (Directory)" << std::endl;
    for (const auto& pair : children_) {
        pair.second->listContents(indent + 1);
    }
}

bool Directory::addChild(std::unique_ptr<FileSystemNode> child) {
    if (!child) return false;
    std::string childName = child->getName();
    if (children_.find(childName) != children_.end()) {
        std::cerr << "Error: Item '" << childName << "' already exists in '" << getName() << "'." << std::endl;
        return false;
    }
    children_[childName] = std::move(child);
    return true;
}

bool Directory::removeChild(const std::string& name) {
    auto it = children_.find(name);
    if (it != children_.end()) {
        children_.erase(it);
        return true;
    }
    return false;
}

FileSystemNode* Directory::getChild(const std::string& name) const {
    auto it = children_.find(name);
    if (it != children_.end()) {
        return it->second.get();
    }
    return nullptr;
}

Directory* Directory::getSubDirectory(const std::string& name) const {
    FileSystemNode* node = getChild(name);
    if (node && node->isDirectory()) {
        return static_cast<Directory*>(node);
    }
    return nullptr;
}

File* Directory::getFile(const std::string& name) const {
    FileSystemNode* node = getChild(name);
    if (node && !node->isDirectory()) {
        return static_cast<File*>(node);
    }
    return nullptr;
}

std::vector<std::string> Directory::getChildNames() const {
    std::vector<std::string> names;
    names.reserve(children_.size());
    for (const auto& pair : children_) {
        names.push_back(pair.first);
    }
    return names;
}

std::unique_ptr<FileSystemNode> Directory::removeChildAndReturn(const std::string& name) {
    auto it = children_.find(name);
    if (it == children_.end()) {
        return nullptr;
    }
    std::unique_ptr<FileSystemNode> ptr = std::move(it->second);
    children_.erase(it);
    return ptr;
}

void Directory::insertChild(const std::string& name, std::unique_ptr<FileSystemNode> child) {
    children_[name] = std::move(child);
}