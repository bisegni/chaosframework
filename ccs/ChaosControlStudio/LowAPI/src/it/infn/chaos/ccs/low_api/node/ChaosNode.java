/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package it.infn.chaos.ccs.low_api.node;

import static it.infn.chaos.ccs.low_api.node.NewBean.PROP_SAMPLE_PROPERTY;
import java.beans.PropertyChangeListener;
import java.beans.PropertyChangeSupport;

/**
 * Define the abstract CHAOS node
 *
 * @author bisegni
 */
public abstract class ChaosNode {

    private PropertyChangeSupport propertySupport = new PropertyChangeSupport(this);
    //! identify an unit server node
        /*!
     A unit server node is a specialized node that act as server for many configurable
     constrol unit.
     */

    static public String NODE_TYPE_UNIT_SERVER = "nt_unit_server";
    //! identify a control unit node
        /*!
     A CU node is a tipical !CHAOS node that act as controller of hardware of
     other chaos node.
     */
    static public String NODE_TYPE_CONTROL_UNIT = "nt_control_unit";
    //! identify an user itnerface node
        /*!
     An user interface node is that node thac can control over node and show
     data from node, tipically a node taht need to monitor the system and 
     send command to change the system state.
     */
    static public String NODE_TYPE_USER_INTERFACE = "nt_user_interface";
    //! identify a data service node
        /*!
     A data service node is a !CHAOS service that manage the query on producer data
     */
    static public String NODE_TYPE_DATA_SERVICE = "nt_data_service";

    //! identify a wan proxy node
        /*!
     A wan proxy node is a node that permit to adapt the wan syncrhonous worls to
     !CHAOS async one.
     */
    static public String NODE_TYPE_WAN_PROXY = "nt_wan_proxy";

    private String nodeUniqueID = "";
    private String nodeType = "";
    private String nodeRpcAddr = "";
    private String nodeRpcDomain = "";
    private Integer nodeTimestamp = 0;

    public ChaosNode(String type) {
        this.nodeType = type;
    }

    public String getNodeUniqueID() {
        return nodeUniqueID;
    }

    public String getNodeParentSet() {
        return nodeParentSet;
    }

    public String getNodeType() {
        return nodeType;
    }

    public String getNodeRpcAddr() {
        return nodeRpcAddr;
    }

    public String getNodeRpcDomain() {
        return nodeRpcDomain;
    }

    public Integer getNodeTimestamp() {
        return nodeTimestamp;
    }
    private String nodeParentSet = null;

    public void setNodeUniqueID(String nodeUniqueID) {
        String oldValue = this.nodeUniqueID;
        this.nodeUniqueID = nodeUniqueID;
        propertySupport.firePropertyChange("node_unique_id", oldValue, nodeUniqueID);
    }

    public void setNodeParentSet(String nodeParentSet) {
        String oldValue = this.nodeParentSet;
        this.nodeParentSet = nodeParentSet;
        propertySupport.firePropertyChange("node_parent_set", oldValue, nodeParentSet);
    }

    public void setNodeRpcAddr(String nodeRpcAddr) {
        String oldValue = this.nodeRpcAddr;
        this.nodeRpcAddr = nodeRpcAddr;
        propertySupport.firePropertyChange("node_parent_set", oldValue, nodeRpcAddr);
    }

    public void setNodeRpcDomain(String nodeRpcDomain) {
        String oldValue = this.nodeRpcDomain;
        this.nodeRpcDomain = nodeRpcDomain;
        propertySupport.firePropertyChange("node_parent_set", oldValue, nodeRpcDomain);
    }

    public void setNodeTimestamp(Integer nodeTimestamp) {
        Integer oldValue = this.nodeTimestamp;
        this.nodeTimestamp = nodeTimestamp;
        propertySupport.firePropertyChange("node_parent_set", oldValue, nodeTimestamp);
    }

    public void addPropertyChangeListener(PropertyChangeListener listener) {
        propertySupport.addPropertyChangeListener(listener);
    }

    public void removePropertyChangeListener(PropertyChangeListener listener) {
        propertySupport.removePropertyChangeListener(listener);
    }

    @Override
    public String toString() {
        return "ChaosNode{" + "nodeUniqueID=" + nodeUniqueID + ", nodeType=" + nodeType + ", nodeRpcAddr=" + nodeRpcAddr + ", nodeRpcDomain=" + nodeRpcDomain + ", nodeTimestamp=" + nodeTimestamp + ", nodeParentSet=" + nodeParentSet + '}';
    }
}
