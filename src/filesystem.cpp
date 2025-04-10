#include "../include/filesystem.h"
#include "../include/directory.h"
#include "../include/file.h"
#include "../include/filesystem_node.h"

#include <iostream>
#include <sstream>
#include <algorithm>

#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#endif

FileSystem::FileSystem() {
    root_ = std::make_unique<Directory>("/", nullptr);
    current_directory_ = root_.get();
}

std::vector<std::string> FileSystem::splitPath(const std::string& path) {
    std::vector<std::string> parts;
    std::stringstream ss(path);
    std::string part;
    while (std::getline(ss, part, '/')) {
        if (!part.empty() && part != ".") {
            parts.push_back(part);
        }
    }
    return parts;
}

FileSystemNode* FileSystem::findNode(const std::string& path) const {
    if (path.empty()) return current_directory_;

    Directory* start_dir = (path[0] == '/') ? root_.get() : current_directory_;
    std::vector<std::string> parts = splitPath(path);

    FileSystemNode* current_node = start_dir;
    for (const std::string& part : parts) {
        if (!current_node || !current_node->isDirectory()) return nullptr;

        Directory* current_dir_node = static_cast<Directory*>(current_node);

        if (part == "..") {
            current_node = current_dir_node->getParent();
            if (!current_node) current_node = root_.get();
        } else {
            current_node = current_dir_node->getChild(part);
            if (!current_node) return nullptr;
        }
    }
    return current_node;
}

Directory* FileSystem::findParentDirectory(const std::string& path) const {
    size_t last_slash = path.find_last_of('/');
    std::string parent_path;

    if (last_slash == std::string::npos) {
        return current_directory_;
    } else if (last_slash == 0) {
        return root_.get();
    } else {
        parent_path = path.substr(0, last_slash);
    }

    FileSystemNode* node = findNode(parent_path);
    if (node && node->isDirectory()) {
        return static_cast<Directory*>(node);
    }
    return nullptr;
}

std::string FileSystem::getBaseName(const std::string& path) {
    if (path.empty() || path == "/") return "/";
    size_t last_slash = path.find_last_of('/');
    if (last_slash == std::string::npos) return path;

    std::string cleaned_path = path;
    if (cleaned_path.back() == '/' && cleaned_path.length() > 1) {
        cleaned_path.pop_back();
        last_slash = cleaned_path.find_last_of('/');
        if (last_slash == std::string::npos) return cleaned_path;
    }
    return cleaned_path.substr(last_slash + 1);
}

std::string FileSystem::pwd() const {
    if (current_directory_ == root_.get()) return "/";

    std::string path = "";
    Directory* temp = current_directory_;
    while (temp != nullptr && temp != root_.get()) {
        path = "/" + temp->getName() + path;
        temp = temp->getParent();
    }
    return (path.empty()) ? "/" : path;
}

void FileSystem::ls(const std::string& path) const {
    FileSystemNode* node = findNode(path);
    if (!node) {
        std::cerr << "ls: cannot access '" << path << "': No such file or directory" << std::endl;
        return;
    }

    if (!node->isDirectory()) {
        std::cout << node->getName() << std::endl;
    } else {
        Directory* dir_node = static_cast<Directory*>(node);
        std::vector<std::string> names = dir_node->getChildNames();
        std::sort(names.begin(), names.end());
        for (const std::string& name : names) {
            FileSystemNode* child = dir_node->getChild(name);
            if (child) {
                std::cout << name << (child->isDirectory() ? "/" : "") << std::endl;
            }
        }
    }
}

bool FileSystem::cd(const std::string& path) {
    if (path == "..") {
        if (current_directory_->getParent() != nullptr) {
            current_directory_ = current_directory_->getParent();
            return true;
        } else {
            return true;
        }
    }

    FileSystemNode* node = findNode(path);
    if (node && node->isDirectory()) {
        current_directory_ = static_cast<Directory*>(node);
        return true;
    } else if (node && !node->isDirectory()) {
        std::cerr << "cd: '" << path << "' is not a directory" << std::endl;
        return false;
    } else {
        std::cerr << "cd: '" << path << "': No such file or directory" << std::endl;
        return false;
    }
}

bool FileSystem::mkdir(const std::string& path) {
    if (path.empty() || path == "/" || path == "." || path == "..") {
        std::cerr << "mkdir: invalid path '" << path << "'" << std::endl;
        return false;
    }

    std::string baseName = getBaseName(path);
    if (baseName.empty() || baseName == "." || baseName == "..") {
        std::cerr << "mkdir: invalid directory name in path '" << path << "'" << std::endl;
        return false;
    }

    Directory* parentDir = findParentDirectory(path);
    if (!parentDir) {
        std::cerr << "mkdir: cannot create directory '" << path << "': Parent directory does not exist" << std::endl;
        return false;
    }

    if (parentDir->getChild(baseName) != nullptr) {
        std::cerr << "mkdir: cannot create directory '" << path << "': File or directory already exists" << std::endl;
        return false;
    }

    auto newDir = std::make_unique<Directory>(baseName, parentDir);
    return parentDir->addChild(std::move(newDir));
}

bool FileSystem::touch(const std::string& path) {
    if (path.empty() || path == "/" || path == "." || path == "..") {
        std::cerr << "touch: invalid path '" << path << "'" << std::endl;
        return false;
    }

    std::string baseName = getBaseName(path);
    if (baseName.empty() || baseName == "." || baseName == "..") {
        std::cerr << "touch: invalid file name in path '" << path << "'" << std::endl;
        return false;
    }

    Directory* parentDir = findParentDirectory(path);
    if (!parentDir) {
        std::cerr << "touch: cannot create file '" << path << "': Directory does not exist" << std::endl;
        return false;
    }

    FileSystemNode* existingNode = parentDir->getChild(baseName);
    if (existingNode != nullptr) {
        if (existingNode->isDirectory()) {
            std::cerr << "touch: cannot create file '" << path << "': A directory with this name already exists" << std::endl;
            return false;
        }
        return true;
    }

    auto newFile = std::make_unique<File>(baseName, parentDir);
    return parentDir->addChild(std::move(newFile));
}

bool FileSystem::rm(const std::string& path) {
    if (path.empty() || path == "/" || path == "." || path == "..") {
        std::cerr << "rm: invalid path '" << path << "'" << std::endl;
        return false;
    }

    std::string baseName = getBaseName(path);
    if (baseName.empty() || baseName == "." || baseName == "..") {
        std::cerr << "rm: invalid name in path '" << path << "'" << std::endl;
        return false;
    }

    Directory* parentDir = findParentDirectory(path);
    if (!parentDir) {
        std::cerr << "rm: cannot remove '" << path << "': No such file or directory" << std::endl;
        return false;
    }

    FileSystemNode* nodeToRemove = parentDir->getChild(baseName);
    if (!nodeToRemove) {
        std::cerr << "rm: cannot remove '" << path << "': No such file or directory" << std::endl;
        return false;
    }

    if (nodeToRemove == current_directory_) {
        std::cerr << "rm: cannot remove current directory '.' " << std::endl;
        return false;
    }

    Directory* checkParent = current_directory_->getParent();
    while (checkParent != nullptr) {
        if (nodeToRemove == checkParent) {
            std::cerr << "rm: cannot remove ancestor directory" << std::endl;
            return false;
        }
        checkParent = checkParent->getParent();
    }

    return parentDir->removeChild(baseName);
}

void FileSystem::cat(const std::string& path) const {
    FileSystemNode* node = findNode(path);
    if (!node) {
        std::cerr << "cat: '" << path << "': No such file or directory" << std::endl;
    } else if (node->isDirectory()) {
        std::cerr << "cat: '" << path << "': Is a directory" << std::endl;
    } else {
        File* fileNode = static_cast<File*>(node);
        std::cout << fileNode->getContent() << std::endl;
    }
}

bool FileSystem::echoToFile(const std::string& content, const std::string& path) {
    FileSystemNode* node = findNode(path);
    std::string baseName = getBaseName(path);
    Directory* parentDir = findParentDirectory(path);

    if (!parentDir) {
        std::cerr << "echo: cannot write to '" << path << "': Directory does not exist" << std::endl;
        return false;
    }

    if (node && node->isDirectory()) {
        std::cerr << "echo: cannot write to '" << path << "': Is a directory" << std::endl;
        return false;
    } else if (node) {
        File* fileNode = static_cast<File*>(node);
        fileNode->setContent(content);
        return true;
    } else {
        if (baseName.empty() || baseName == "." || baseName == "..") {
            std::cerr << "echo: invalid file name in path '" << path << "'" << std::endl;
            return false;
        }
        auto newFile = std::make_unique<File>(baseName, parentDir);
        newFile->setContent(content);
        return parentDir->addChild(std::move(newFile));
        }
    }

bool FileSystem::rename(const std::string& path, const std::string& newName) {
    if (newName.empty() || newName == "." || newName == "..") {
        std::cerr << "rename: invalid new name '" << newName << "'" << std::endl;
        return false;
    }

    FileSystemNode* node = findNode(path);
    if (!node) {
        std::cerr << "rename: cannot rename '" << path << "': No such file or directory" << std::endl;
        return false;
    }

    Directory* parentDir = node->getParent();
    if (!parentDir) {
        std::cerr << "rename: cannot rename root directory" << std::endl;
        return false;
    }

    if (parentDir->getChild(newName) != nullptr) {
        std::cerr << "rename: target name '" << newName << "' already exists in directory" << std::endl;
        return false;
    }

    std::string oldName = node->getName();
    std::unique_ptr<FileSystemNode> temp = parentDir->removeChildAndReturn(oldName);
    if (!temp) {
        std::cerr << "rename: internal error, node not found in parent" << std::endl;
        return false;
    }

    temp->rename(newName);
    parentDir->insertChild(newName, std::move(temp));
    return true;
}

void FileSystem::printTree() const {
    std::cout << "--- File System Tree ---" << std::endl;
    root_->listContents(0);
    std::cout << "------------------------" << std::endl;
}

void FileSystem::neofetch() {
#ifdef _WIN32
    std::string os_name = "Windows";

    PROCESS_MEMORY_COUNTERS pmc;
    HANDLE hProcess = GetCurrentProcess();
    SIZE_T memUsageKB = 0;

    if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))) {
        memUsageKB = pmc.WorkingSetSize / 1024;
    }

    std::cout << "==============================" << std::endl;
    std::cout << "         NEOFETCH INFO        " << std::endl;
    std::cout << "==============================" << std::endl;
    std::cout << "Sistema Operacional: " << os_name << std::endl;
    std::cout << "Memória usada pelo processo: " << memUsageKB << " KB" << std::endl;
    std::cout << "==============================" << std::endl;
#else
    std::cout << "==============================" << std::endl;
    std::cout << "         NEOFETCH INFO        " << std::endl;
    std::cout << "==============================" << std::endl;
    std::cout << "Sistema Operacional: Linux/Unix" << std::endl;
    std::cout << "Uso de memória não implementado para este sistema." << std::endl;
    std::cout << "==============================" << std::endl;
#endif
}

Directory* FileSystem::getCurrentDirectory() const {
    return current_directory_;
}