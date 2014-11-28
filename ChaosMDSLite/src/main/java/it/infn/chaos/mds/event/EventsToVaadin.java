package it.infn.chaos.mds.event;

import it.infn.chaos.mds.LiveDataPreferenceView;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.Observable;

import org.ref.common.mvc.ViewNotifyEvent;
import org.ref.server.auth.RefAuthneticatorListener;
import org.ref.server.webapp.RefVaadinApplicationController;
import org.ref.server.webapp.RefVaadinBasePanel;

import com.vaadin.ui.AbstractComponent;
import com.vaadin.ui.CustomComponent;

import it.infn.chaos.mds.event.VaadinEvent;

public class EventsToVaadin  extends CustomComponent{
	public static final String	CHAOS_EVENT_UPDATE_DEVICE_LIST					= "CHAOS_EVENT_UPDATE_DEVICE_LIST";
	public static final String  CHAOS_EVENT_CU_REGISTERED 						= "CHAOS_EVENT_CU_REGISTERED";
	public static final String  CHAOS_EVENT_UI_REGISTERED 					= "CHAOS_EVENT_UI_REGISTERED";
	public static final String  CHAOS_EVENT_ERROR				= "CHAOS_EVENT_ERROR";

	private List list = new ArrayList();
	private static EventsToVaadin unique=null;
	private static Integer nevents = 0; 
	
	public static Integer getEvents(){
		return nevents;
	}
	public void addListener(RefVaadinApplicationController l){
		list.add(l);
	}
	public void removeListener(RefVaadinApplicationController l){
		list.remove(l);
		
	}
	
	private synchronized void fire(Observable source, Object sourceData) {
		Iterator i = list.iterator();
		nevents++;
		while(i.hasNext()){
			((RefVaadinApplicationController)i.next()).update(source, sourceData);
		}
		
	}
	public void deviceRegistrationEvent(Object data){
		VaadinEvent ev = new VaadinEvent(CHAOS_EVENT_CU_REGISTERED,data);
		fire(null,ev);
	}
	
	public void unitRegistrationEvent(Object data){
		VaadinEvent ev = new VaadinEvent(CHAOS_EVENT_UI_REGISTERED,data);
		fire(null,ev);
	}
	
	public void errorEvent(Object data){
		VaadinEvent ev = new VaadinEvent(CHAOS_EVENT_ERROR,data);
		fire(null,ev);
	}
	public synchronized static EventsToVaadin getInstance(){
		if(unique==null){
			unique = new EventsToVaadin();
			
		}
		return unique;
	}
}
