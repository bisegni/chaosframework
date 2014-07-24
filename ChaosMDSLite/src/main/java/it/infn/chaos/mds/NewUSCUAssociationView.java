/**
 * 
 */
package it.infn.chaos.mds;

import java.util.Observable;

import org.ref.common.mvc.ViewNotifyEvent;
import org.ref.server.webapp.RefVaadinBasePanel;

/**
 * @author bisegni
 * 
 */
public class NewUSCUAssociationView extends RefVaadinBasePanel implements com.vaadin.data.Container.PropertySetChangeListener {
	private static final long			serialVersionUID	= -841710385419733478L;
	public static final String			SET_SC_CUTYPE_VALUE	= "SET_SC_CUTYPE_VALUE";
	private NewUSCUAssociationVaadin	impl				= new NewUSCUAssociationVaadin();
	private boolean						editingServer		= false;

	/*
	 * (non-Javadoc)
	 * 
	 * @see org.ref.server.webapp.RefVaadinBasePanel#initGui()
	 */
	@SuppressWarnings("serial")
	@Override
	public void initGui() {
		addComponent(impl);
		/*
		 * liveDataView.getButtonClose().addListener(new ClickListener() { public void buttonClick(ClickEvent event) { notifyEventoToControllerWithData(EVENT_PREFERENCE_CLOSE_VIEW, event.getSource(), null); }
		 * 
		 * });
		 */

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
			if (viewEvent.getEventKind().equals(NewUSCUAssociationView.SET_SC_CUTYPE_VALUE)) {
				impl.getUsSelected().setValue(((String[])viewEvent.getEventData())[0]);
				impl.getCuTypeSelected().setValue(((String[])viewEvent.getEventData())[1]);
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

	/**
	 * @return the editingServer
	 */
	public boolean isEditingServer() {
		return editingServer;
	}

	/**
	 * @param editingServer
	 *            the editingServer to set
	 */
	public void setEditingServer(boolean editingServer) {
		this.editingServer = editingServer;
	}

}
