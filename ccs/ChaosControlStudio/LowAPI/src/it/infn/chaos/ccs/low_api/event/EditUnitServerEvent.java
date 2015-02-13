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
public class EditUnitServerEvent extends AbstractEvent {

    public static final String EVENT_ID = "it.infn.chaos.css.low_api.event.EditUnitServerEvent";

    public EditUnitServerEvent(ChaosNode eventTarget) {
        super(EVENT_ID, eventTarget);
    }
}
