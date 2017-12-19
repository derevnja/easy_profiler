#ifndef EASY_PROFILER_CONVERTER_H
#define EASY_PROFILER_CONVERTER_H

///std
#include <fstream>
#include <vector>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>

///this
#include <easy/easy_protocol.h>
#include <easy/reader.h>

namespace profiler{

namespace reader {

class BlocksTreeNode
{
public:
    ::std::shared_ptr<BlockInfo> current_block;
    BlocksTreeNode* parent;
    ::std::vector<::std::shared_ptr<BlocksTreeNode>> children;

    BlocksTreeNode(BlocksTreeNode&& other)
       : current_block(::std::move(other.current_block)),
         parent(other.parent),
         children(::std::move(other.children))
    {
    }
    BlocksTreeNode():current_block(nullptr),
                     parent(nullptr)
    {
    }
};

typedef ::std::unordered_map<::profiler::thread_id_t, BlocksTreeNode, ::profiler::passthrough_hash<::profiler::thread_id_t> > thread_blocks_tree_t;
typedef ::std::unordered_map<::profiler::thread_id_t, ::std::string> thread_names_t;


class FileReader EASY_FINAL
{
public:
    typedef ::std::vector<::std::shared_ptr<BlocksTreeNode> >      TreeNodes;
    typedef ::std::vector<::std::shared_ptr<ContextSwitchEvent> >  ContextSwitches;
    typedef ::std::vector<::std::shared_ptr<BlockInfo> >           Events;

    FileReader()
    { }

    ~FileReader()
    { }

    void                        readFile(const ::std::string& filename);
    ///get blocks tree
    const thread_blocks_tree_t&            getBlocksTreeData();
    /*! get thread name by Id
    \param threadId thread Id
    \return Name of thread
    */
    std::string getThreadName(uint64_t threadId);
    /*! get file version
    \return data file version
    */
    uint32_t getVersion();

private:
    /*! Operate with data after fillTreesFromFile(...) function call*/
    void               prepareData();

    /*! Organize all InfoBlocks to tree with input BlocksTreeElement as a root
    \param &element root element where we shall insert other elements
    \param Id block id in a common set of blocks
    */
    void               prepareBlocksInfo(::std::shared_ptr<BlocksTreeNode> &element, uint32_t Id);
    ::profiler::block_index_t parseLogInfo(const std::string &filename,
                                           ::std::stringstream& _log);


    ::profiler::SerializedData                             serialized_blocks, serialized_descriptors;
    ::std::stringstream                                    errorMessage;
    thread_blocks_tree_t                                   m_BlocksTree2;
    thread_names_t                                         m_threadNames;
    ContextSwitches                                        m_ContextSwitches;
    Events                                                 m_events;
    std::vector<std::shared_ptr<BlockDescriptor>>          m_BlockDescriptors;
    uint32_t                                               m_version;
};

} //namespace reader
} //namespace profiler
#endif
