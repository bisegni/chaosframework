/**
 * 
 */
package it.infn.chaos.mds;

import it.infn.chaos.mds.business.UnitServer;

import java.util.Observable;

import org.ref.common.mvc.ViewNotifyEvent;
import org.ref.server.webapp.RefVaadinBasePanel;

import com.vaadin.event.ItemClickEvent;
import com.vaadin.event.ItemClickEvent.ItemClickListener;
import com.vaadin.ui.Button.ClickEvent;
import com.vaadin.ui.Button.ClickListener;

/**
 * @author bisegni
 * 
 */
public class USEditInfoView extends RefVaadinBasePanel implements com.vaadin.data.Container.PropertySetChangeListener, ItemClickListener {
	private static final long	serialVersionUID		= -841710385419733478L;
	public static final String	EVENT_SAVE				= "EVENT_SAVE";
	public static final String	EVENT_CANCELL			= "EVENT_CANCELL";
	public static final String	EVENT_SET_UNIT_SERVER	= "EVENT_SET_UNIT_SERVER";
	private USEditInfoVaadin			impl					= new USEditInfoVaadin();
	private UnitServer			workingUnitServer		= null;

	/*
	 * (non-Javadoc)
	 * 
	 * @see org.ref.server.webapp.RefVaadinBasePanel#initGui()
	 */
	@SuppressWarnings("serial")
	@Override
	public void initGui() {
		addComponent(impl);

		impl.getButtonCancell().addListener(new ClickListener() {
			public void buttonClick(ClickEvent event) {
				notifyEventoToControllerWithData(USEditInfoView.EVENT_CANCELL, event.getSource(), null);
			}
		});
		impl.getButtonSaveInfo().addListener(new ClickListener() {
			public void buttonClick(ClickEvent event) {
				//get new alias value
				workingUnitServer.setAlias(impl.getTextFieldUnitServerAlias().getValue().toString());
				
				//send save event
				notifyEventoToControllerWithData(USEditInfoView.EVENT_SAVE, event.getSource(), workingUnitServer);
			}
		});

	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see org.ref.server.webapp.RefVaadinBasePanel#update(java.util.Observable, java.lang.Object)
	 */
	@Override
	public void update(Observable source, Object sourceData) {
		ViewNotifyEvent viewEvent = null;
		if (sourceData instanceof ViewNotifyEvent) {
			viewEvent = (ViewNotifyEvent) sourceData;
			if(viewEvent.getEventKind().equals(USEditInfoView.EVENT_SET_UNIT_SERVER)){
				workingUnitServer = (UnitServer) viewEvent.getEventData();
				impl.getTextFieldUnitServerAlias().setValue(workingUnitServer.getAlias());
			}
		}
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.vaadin.data.Container.PropertySetChangeListener#containerPropertySetChange(com.vaadin.data.Container.PropertySetChangeEvent)
	 */
	public void containerPropertySetChange(com.vaadin.data.Container.PropertySetChangeEvent event) {
		System.out.println(event);

	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.vaadin.event.ItemClickEvent.ItemClickListener#itemClick(com.vaadin.event.ItemClickEvent)
	 */
	public void itemClick(ItemClickEvent event) {

	}

	/**
	 * 
	 * @param itemID
	 */
	private void associationSeleteionHasChanged(Object itemID) {
	}
}
