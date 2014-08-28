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
	public static final String	EVENT_UPDATE_DEVICE_LIST					= "MDSAppView_EVENT_UPDATE_DEVICE_LIST";
	public static final String  EVENT_CU_REGISTERED 						= "MDSAppView_EVENT_CU_REGISTERED";
	private List list = new ArrayList();
	private static EventsToVaadin unique=null;
	
	public void addListener(RefVaadinApplicationController l){
		list.add(l);
	}
	public void removeListener(RefVaadinApplicationController l){
		list.remove(l);
		
	}
	
	private  void fire(Observable source, Object sourceData) {
		Iterator i = list.iterator();
		while(i.hasNext()){
			((RefVaadinApplicationController)i.next()).update(source, sourceData);
		}
		
	}
	public void deviceRegistrationEvent(){
		VaadinEvent ev = new VaadinEvent(EVENT_CU_REGISTERED);
		fire(null,ev);
		requestRepaint();
	}
	
	public static EventsToVaadin getInstance(){
		if(unique==null){
			unique = new EventsToVaadin();
			
		}
		return unique;
	}
}
