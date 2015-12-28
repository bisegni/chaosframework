/*
 *	TreeGroupDataAccess.cpp
 *	!CHAOS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2015 INFN, National Institute of Nuclear Physics
 *
 *    	Licensed under the Apache License, Version 2.0 (the "License");
 *    	you may not use this file except in compliance with the License.
 *    	You may obtain a copy of the License at
 *
 *    	http://www.apache.org/licenses/LICENSE-2.0
 *
 *    	Unless required by applicable law or agreed to in writing, software
 *    	distributed under the License is distributed on an "AS IS" BASIS,
 *    	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    	See the License for the specific language governing permissions and
 *    	limitations under the License.
 */

#include "TreeGroupDataAccess.h"

#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>

#define SDA_INFO INFO_LOG(TreeGroupDataAccess)
#define SDA_DBG  DBG_LOG(TreeGroupDataAccess)
#define SDA_ERR  ERR_LOG(TreeGroupDataAccess)

const boost::regex NodePathRegularExpression("(\\/[a-zA-Z0-9_.]+)+");
const boost::regex NodeNameRegularExpression("([a-zA-Z0-9_.])+");


using namespace chaos::metadata_service::persistence::data_access;

DEFINE_DA_NAME(TreeGroupDataAccess)


//! default constructor
TreeGroupDataAccess::TreeGroupDataAccess():
AbstractDataAccess("TreeGroupDataAccess"){}

//!default destructor
TreeGroupDataAccess::~TreeGroupDataAccess(){}


//check the three path
bool TreeGroupDataAccess::checkPathSintax(const std::string& tree_path) {
    bool result = false;
    boost::cmatch what;
    if(!(result = boost::regex_match(tree_path.c_str(), what, NodePathRegularExpression))) {
        //error recognizing the path
        SDA_ERR << "The tree path " << tree_path << " is not well formed";
    }
    return result;
}

//check the three path
bool TreeGroupDataAccess::checkNodeNameSintax(const std::string& node_name) {
    bool result = false;
    boost::cmatch what;
    if(!(result = boost::regex_match(node_name.c_str(), what, NodeNameRegularExpression))) {
        //error recognizing the path
        SDA_ERR << "The node name " << node_name << " is not well formed";
    }
    return result;
}

bool TreeGroupDataAccess::estractNodeFromPath(const std::string& node_path,
                                              std::string& node_name,
                                              std::string& parent_path) {
    if(!checkPathSintax(node_path)) return false;
    std::vector< std::string > tree_path_node;
    // split the path in the node elements the first slash in path realize an empty string as
    // first element in tree_path_node vector so root only path give a vector of two element
    // with an empty string as first element
    boost::split(tree_path_node,
                 node_path, boost::is_any_of("/"),
                 boost::token_compress_on );
    //estract the node name that is the element in the front of the vector
    node_name = tree_path_node.back();
    
    //remove node name and slash from path for get parent path
    parent_path = node_path;
    parent_path.resize(parent_path.size()-(node_name.size()+1));
    return true;
}