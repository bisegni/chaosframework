/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package it.infn.chaos.ccs.low_api.node;

import java.util.ArrayList;
import java.util.UUID;

/**
 * Define the Unit Server chaos nodes sublcass
 * @author bisegni
 */
public class UnitServerNode extends ChaosNode {
    private final ArrayList<String> controlUnitTypeList = new ArrayList<String>();

    public UnitServerNode() {
         super(ChaosNode.NODE_TYPE_UNIT_SERVER);
         
         //generate a temporary unique id
         setNodeUniqueID(UUID.randomUUID().toString());
    }
    
    public ArrayList<String> getControlUnitTypeList() {
        return controlUnitTypeList;
    }
    
    public void addControlUnitType(String controlUnitType) {
        controlUnitTypeList.add(controlUnitType);
    }
    
    public void clearControlUnitTypeList() {
        controlUnitTypeList.clear();
    }
    
}
