package it.infn.chaos.mds.event;

import java.io.Serializable;
import java.util.ArrayList;
import java.util.List;

import com.vaadin.ui.AbstractComponent;

/*
 * @author Andrea Michelotti
 * */
public class ChaosEventComponent extends AbstractComponent {
	private static ChaosEventComponent unique = null;
	public static final String	CHAOS_EVENT_UPDATE_DEVICE_LIST					= "CHAOS_EVENT_UPDATE_DEVICE_LIST";
	public static final String  CHAOS_EVENT_CU_REGISTERED 						= "CHAOS_EVENT_CU_REGISTERED";
	public static final String  CHAOS_EVENT_UI_REGISTERED 					= "CHAOS_EVENT_UI_REGISTERED";
	public static final String  CHAOS_EVENT_ERROR				= "CHAOS_EVENT_ERROR";
	
	public interface ChaosEventListener extends Serializable {
	    public void event(VaadinEvent source);
	  }
	 private final List<ChaosEventListener> chaosListeners = new ArrayList<ChaosEventListener>();
	 
	 
	 public void addListener(ChaosEventListener l){
		 chaosListeners.add(l);
	 }
	 public void removeListener(ChaosEventListener l){
		 chaosListeners.remove(l);
	 }
	 protected void fire(VaadinEvent ev){
		 for(ChaosEventListener l : chaosListeners){
			 l.event(ev);
		 }
	 }
	 
	 public synchronized static ChaosEventComponent getInstance(){
			if(unique==null){
				unique = new ChaosEventComponent();
				
			}
			return unique;
		}
	 
		public void deviceRegistrationEvent(Object data){
			VaadinEvent ev = new VaadinEvent(CHAOS_EVENT_CU_REGISTERED,data);
			fire(ev);
		}
		
		public void unitRegistrationEvent(Object data){
			VaadinEvent ev = new VaadinEvent(CHAOS_EVENT_UI_REGISTERED,data);
			fire(ev);
		}
		
		public void errorEvent(Object data){
			VaadinEvent ev = new VaadinEvent(CHAOS_EVENT_ERROR,data);
			fire(ev);
		}
}
