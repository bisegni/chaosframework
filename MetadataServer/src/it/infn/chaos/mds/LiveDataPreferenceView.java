/**
 * 
 */
package it.infn.chaos.mds;

import java.util.Observable;

import org.ref.server.webapp.RefVaadinBasePanel;

import com.vaadin.event.ItemClickEvent;
import com.vaadin.event.ItemClickEvent.ItemClickListener;

/**
 * @author bisegni
 * 
 */
public class LiveDataPreferenceView extends RefVaadinBasePanel implements ItemClickListener {
	private LiveDataPreferenceVaadin	liveDataView			= new LiveDataPreferenceVaadin();
	/**
	 * 
	 */
	private static final long			serialVersionUID		= -841710385419733478L;
	public static final String			KEY_PREFERENCE_TABLE	= "KEY_PREFERENCE_TABLE";

	/*
	 * (non-Javadoc)
	 * 
	 * @see org.ref.server.webapp.RefVaadinBasePanel#initGui()
	 */
	@Override
	public void initGui() {
		addComponent(liveDataView);
		liveDataView.getPreferenceTable().addListener(this);
		setComponentKey(KEY_PREFERENCE_TABLE, liveDataView.getPreferenceTable());

	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see org.ref.server.webapp.RefVaadinBasePanel#update(java.util.Observable, java.lang.Object)
	 */
	@Override
	public void update(Observable source, Object sourceData) {

	}

	@Override
	public void itemClick(ItemClickEvent event) {
	}

}
