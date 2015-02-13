/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package it.infn.chaos.ccs.low_api.event;

import it.infn.chaos.ccs.low_api.node.ChaosNode;

/**
 *
 * @author bisegni
 */
public abstract class AbstractEvent {

    private String eventID = null;
    private ChaosNode eventTarget = null;

    public AbstractEvent(String eventID, ChaosNode eventTarget) {
        this.eventID = eventID;
        this.eventTarget = eventTarget;
    }

    public ChaosNode getEventTarget() {
        return eventTarget;
    }

    public String getEventID() {
        return eventID;
    }

}
