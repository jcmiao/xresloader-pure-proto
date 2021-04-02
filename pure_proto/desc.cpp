#include "desc.h"
#include <iostream>
#include <fstream>
#include <string>
#include <direct.h>  
#include <algorithm>
#include <map>

#include "xrescode_extensions_v3.pb.h"
#include "v3/xresloader.pb.h"
#include "v3/xresloader_ue.pb.h"
#include "xrescode_extensions_v3.pb.h"

FileGenerator::FileGenerator(const google::protobuf::FileDescriptorProto& fdesc)
    : m_fdesc(fdesc)
{
    
}


Tool::Tool()
{
    // TODO: 改从配置文件读
    m_filterSet.insert("pb_header_v3.proto");
    m_filterSet.insert("pb_header.proto");
    m_filterSet.insert("xresloader_ue.proto");
    m_filterSet.insert("xresloader.proto");
    m_filterSet.insert("xrescode_extensions_v3.proto");
    m_filterSet.insert("descriptor.proto");
}

void Tool::Do(const std::string& pbfile, const std::string& outpath)
{
    m_pbfilepath = pbfile;
    m_outpath = outpath;

    Init();
    ParseProto();
}

void Tool::Init()
{
    if (access(m_outpath.c_str(), 0) == -1)
    {
        mkdir(m_outpath.c_str()); 
    }
}

void Tool::ParseProto()
{
    std::fstream fin;
    fin.open(m_pbfilepath, std::ios::in | std::ios::binary);
    if (!fin.is_open())
    {
        std::cout << "open " << m_pbfilepath << " failed" << std::endl;
        return;
    }
    m_fds.ParseFromIstream(&fin);
    for (auto& file : m_fds.file())
    {
        GenerateOneFile(file);
    }    
}

class BuildFileErrorCollector2 : public google::protobuf::DescriptorPool::ErrorCollector {
 public:
  BuildFileErrorCollector2() : error_message(""), had_errors_(false) {}

  void AddError(const std::string& filename, const std::string& element_name,
                const google::protobuf::Message* descriptor, ErrorLocation location,
                const std::string& message) override {
    // Replicates the logging behavior that happens in the C++ implementation
    // when an error collector is not passed in.
    if (!had_errors_) {
      error_message +=
          ("Invalid proto descriptor for file \"" + filename + "\":\n");
      had_errors_ = true;
    }
    // As this only happens on failure and will result in the program not
    // running at all, no effort is made to optimize this string manipulation.
    error_message += ("  " + element_name + ": " + message + "\n");
  }

  void Clear() {
    had_errors_ = false;
    error_message = "";
  }

  std::string error_message;

 private:
  bool had_errors_;
};

void Tool::GenerateOneFile(const google::protobuf::FileDescriptorProto& fdesc)
{
    if (IsFilterFile(fdesc.name()))
    {
        return;
    }

    google::protobuf::FileDescriptorProto temDesc = fdesc;
    auto depends = temDesc.dependency();
    temDesc.clear_dependency();
    for (auto& d : depends)
    {
        if (IsFilterFile(d))
        {

        }
        else 
        {
            temDesc.add_dependency(d);
        }
    }

    ClearFileOption(temDesc);
    for (auto& msg : *temDesc.mutable_message_type())
    {
        ClearOptionAndJsonName(msg);
    }

    for (auto& e : *temDesc.mutable_enum_type())
    {
        ClearEnumOption(e);
    }

    BuildFileErrorCollector2 bu2;
    const google::protobuf::FileDescriptor* fd = m_pool.BuildFileCollectingErrors(temDesc, &bu2);
    if (!fd)
    {
        std::cout << bu2.error_message << std::endl;
        return;
    }
    google::protobuf::DebugStringOptions o;
    // o.elide_group_body = true;
    // o.elide_oneof_body = true;
    // o.include_comments = true;
    std::string descStr = fd->DebugStringWithOptions(o);
    // std::cout << fd->DebugStringWithOptions(o) << std::endl;
    std::ofstream fout;
    fout.open(m_outpath + "/" + fdesc.name(), std::ios::out);
    fout.write(descStr.c_str(), descStr.size());
    fout.close();
}

void Tool::ClearOptionAndJsonName(google::protobuf::DescriptorProto& msg)
{
    ClearMsgOption(msg);
    
    for (auto& field : *msg.mutable_field())
    {
        ClearFieldOption(field);
        field.clear_json_name();
    }

    for (auto& e : *msg.mutable_enum_type())
    {
        ClearEnumOption(e);
    }

    for (auto& nmsg : *msg.mutable_nested_type())
    {
        ClearOptionAndJsonName(nmsg);
    }
}

void Tool::ClearEnumOption(google::protobuf::EnumDescriptorProto& e)
{
    e.mutable_options()->ClearExtension(org::xresloader::enum_description);
    for (auto& v : *e.mutable_value())
    {
        v.mutable_options()->ClearExtension(org::xresloader::enumv_description);
        v.mutable_options()->ClearExtension(org::xresloader::enum_alias);
    }
}

void Tool::ClearFileOption(google::protobuf::FileDescriptorProto& fdesc)
{
    fdesc.mutable_options()->ClearExtension(org::xresloader::file_description);
}
void Tool::ClearMsgOption(google::protobuf::DescriptorProto& msg)
{
    //xresloader.proto
    msg.mutable_options()->ClearExtension(org::xresloader::msg_description);
    msg.mutable_options()->ClearExtension(org::xresloader::msg_require_mapping_all);
    msg.mutable_options()->ClearExtension(org::xresloader::msg_separator);

    //xresloader_ue.proto
    msg.mutable_options()->ClearExtension(org::xresloader::ue::helper);
    msg.mutable_options()->ClearExtension(org::xresloader::ue::not_data_table);

    //xrescode.proto
    msg.mutable_options()->ClearExtension(xrescode::loader);
}

void Tool::ClearFieldOption(google::protobuf::FieldDescriptorProto& field)
{
    //xresloader.proto
    field.mutable_options()->ClearExtension(org::xresloader::verifier);
    field.mutable_options()->ClearExtension(org::xresloader::field_description);
    field.mutable_options()->ClearExtension(org::xresloader::field_alias);
    field.mutable_options()->ClearExtension(org::xresloader::field_ratio);
    field.mutable_options()->ClearExtension(org::xresloader::field_separator);
    field.mutable_options()->ClearExtension(org::xresloader::field_required);
    field.mutable_options()->ClearExtension(org::xresloader::field_tags);

    //xresloader_ue.proto
    field.mutable_options()->ClearExtension(org::xresloader::ue::key_tag);
    field.mutable_options()->ClearExtension(org::xresloader::ue::ue_type_name);
    field.mutable_options()->ClearExtension(org::xresloader::ue::ue_type_is_class);
}

bool Tool::IsFilterFile(const std::string& fileName)
{
    for (auto& str : m_filterSet)
    {
        if (fileName.find(str) != std::string::npos)
        {
            return true;
        }
    }
    return false;
}