#include "../include/file.h"
#include <iostream>

File::File(const std::string& name, Directory* parent)
    : FileSystemNode(name, parent), content_("") {}

bool File::isDirectory() const {
    return false;
}

void File::listContents(int indent) const {
    printIndent(indent);
    std::cout << "- " << getName() << " (File, size=" << content_.length() << ")" << std::endl;
}

void File::setContent(const std::string& content) {
    content_ = content;
}

const std::string& File::getContent() const {
    return content_;
}