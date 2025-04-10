#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <memory>
#include <string>
#include <vector>

class FileSystemNode;
class Directory;

class FileSystem {
public:
    FileSystem();
    FileSystem(const FileSystem&) = delete;
    FileSystem& operator=(const FileSystem&) = delete;

    static std::vector<std::string> splitPath(const std::string& path);
    FileSystemNode* findNode(const std::string& path) const;
    Directory* findParentDirectory(const std::string& path) const;
    static std::string getBaseName(const std::string& path);

    std::string pwd() const;
    void ls(const std::string& path = ".") const;
    bool cd(const std::string& path);
    bool mkdir(const std::string& path);
    bool touch(const std::string& path);
    bool rm(const std::string& path);
    void cat(const std::string& path) const;
    bool echoToFile(const std::string& content, const std::string& path);
    bool rename(const std::string& path, const std::string& newName);
    void printTree() const;
    void neofetch();

    Directory* getCurrentDirectory() const;

private:
    std::unique_ptr<Directory> root_;
    Directory* current_directory_;
};

#endif // FILESYSTEM_H