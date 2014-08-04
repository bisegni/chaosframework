/**
 * 
 */
package it.infn.chaos.mds;

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
public class USAddCuTypeView extends RefVaadinBasePanel implements com.vaadin.data.Container.PropertySetChangeListener, ItemClickListener {
	private static final long	serialVersionUID		= -841710385419733478L;
	public static final String	EVENT_SAVE				= "USAddCuTypeView_EVENT_SAVE";
	public static final String	EVENT_CANCELL			= "USAddCuTypeView_EVENT_CANCELL";
	public static final String	EVENT_SET_UNIT_SERVER	= "USAddCuTypeView_EVENT_SET_UNIT_SERVER";
	private USAddCuTypeVaadin	impl					= new USAddCuTypeVaadin();
	private String				workingUnitServerAlias	= null;

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
				notifyEventoToControllerWithData(USAddCuTypeView.EVENT_CANCELL, event.getSource(), null);
			}
		});
		impl.getButtonSaveInfo().addListener(new ClickListener() {
			public void buttonClick(ClickEvent event) {
				// get new alias value
				String[] data = new String[2];
				data[0] = workingUnitServerAlias;
				data[1] = impl.getTextFieldUnitTypeName().getValue().toString();
				// send save event
				notifyEventoToControllerWithData(USAddCuTypeView.EVENT_SAVE, event.getSource(), data);
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
			if (viewEvent.getEventKind().equals(USAddCuTypeView.EVENT_SET_UNIT_SERVER)) {
				workingUnitServerAlias = viewEvent.getEventData().toString();
				impl.setCaption(String.format("New Unit Type for %s", workingUnitServerAlias));
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
