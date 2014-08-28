package it.infn.chaos.mds.event;

import org.ref.common.mvc.ViewNotifyEvent;


public class VaadinEvent extends ViewNotifyEvent {
	private String event;
	
	
	public VaadinEvent(String event_type){
		super(null,null);
		event = event_type;
	}
	
	public String getEventKind(){return event;}
	
}