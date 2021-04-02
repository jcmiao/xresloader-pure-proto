#ifndef _DESC_H_
#define _DESC_H_

#include <string>
#include <list>
#include <set>
#include "google/protobuf/descriptor.pb.h"

struct FileGenerator
{
    FileGenerator(const google::protobuf::FileDescriptorProto& fdesc);

    const google::protobuf::FileDescriptorProto& m_fdesc;
    std::string m_content;
    std::string m_fileName;
    std::string m_fileNameNoSuffix;
    std::set<std::string> m_writeSet;
};

class Tool
{
public:
    Tool();
    void Do(const std::string& pbfile, const std::string& outpath);

private:
    void Init();
    void ParseProto();
    void GenerateOneFile(const google::protobuf::FileDescriptorProto& fdesc);
    void ClearOptionAndJsonName(google::protobuf::DescriptorProto& msg);
    void ClearEnumOption(google::protobuf::EnumDescriptorProto& e);
    void ClearFileOption(google::protobuf::FileDescriptorProto& fdesc);
    void ClearMsgOption(google::protobuf::DescriptorProto& msg);
    void ClearFieldOption(google::protobuf::FieldDescriptorProto& field);



    bool IsFilterFile(const std::string& fileName);

private:
    std::string m_pbfilepath;
    std::string m_outpath;

    std::set<std::string> m_filterSet;
    google::protobuf::FileDescriptorSet m_fds;
    google::protobuf::DescriptorPool m_pool;
};

#endif /* _DESC_H_ */