package it.infn.chaos.mds.event;

import org.ref.common.mvc.ViewNotifyEvent;


public class VaadinEvent extends ViewNotifyEvent {
	private String event;
	private Object data;
	
	public VaadinEvent(String event_type,Object d){
		super(null,null);
		event = event_type;
		data = d;
	}
	public Object getData(){return data;}
	public String getEventKind(){return event;}
	
}