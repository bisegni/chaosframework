/**
 * 
 */
package it.infn.chaos.mds;

import it.infn.chaos.mds.business.DatasetAttribute;
import it.infn.chaos.mds.business.DriverSpec;
import it.infn.chaos.mds.business.UnitServerCuInstance;

import java.util.Collection;
import java.util.Observable;
import java.util.Set;
import java.util.Vector;

import org.ref.common.mvc.ViewNotifyEvent;
import org.ref.server.webapp.RefVaadinBasePanel;
import org.ref.server.webapp.dialog.RefVaadinErrorDialog;

import com.sun.activation.viewers.TextEditor;
import com.vaadin.data.Item;
import com.vaadin.event.Action;
import com.vaadin.event.ItemClickEvent;
import com.vaadin.event.ItemClickEvent.ItemClickListener;
import com.vaadin.ui.Button.ClickEvent;
import com.vaadin.ui.Button.ClickListener;
import com.vaadin.ui.Label;
import com.vaadin.ui.Table;
import com.vaadin.ui.TextArea;
import com.vaadin.ui.TextField;

/**
 * @author bisegni
 * 
 */
public class USCUAssociationListView extends RefVaadinBasePanel implements com.vaadin.data.Container.PropertySetChangeListener, ItemClickListener {
	private static final long				serialVersionUID				= -841710385419733478L;
	static final Action						ACTION_EDIT						= new Action("Edit");
	static final Action						ACTION_SAVE						= new Action("Save");
	static final Action[]					ACTIONS_TO_EDIT					= new Action[] { ACTION_EDIT };
	static final Action[]					ACTIONS_IN_EDIT					= new Action[] { ACTION_SAVE };
	public static final String				SET_US_ALIAS					= "SET_US_ALIAS";
	public static final String				EVENT_REMOVE_ASSOCIATION		= "EVENT_REMOVE_ASSOCIATION";
	public static final String				EVENT_LOAD_INSTANCE				= "EVENT_LOAD_INSTANCE";
	public static final String				EVENT_UNLOAD_INSTANCE			= "EVENT_UNLOAD_INSTANCE";
	public static final String				EVENT_UPDATE_LIST				= "EVENT_UPDATE_LIST";
	private static final Object				TABLE_COLUMN_WU_ID				= "Unique ID";
	private static final Object				TABLE_COLUMN_WU_TYPE			= "type";
	private static final Object				TABLE_COLUMN_WU_STATE			= "State";
	private USCUAssociationListVaadin		impl							= new USCUAssociationListVaadin();
	private boolean							editingServer					= false;
	private Vector<UnitServerCuInstance>	currentInstancesForUnitServer	= null;

	/*
	 * (non-Javadoc)
	 * 
	 * @see org.ref.server.webapp.RefVaadinBasePanel#initGui()
	 */
	@SuppressWarnings("serial")
	@Override
	public void initGui() {
		addComponent(impl);
		impl.getTableAssociation().addListener((ItemClickListener) this);
		impl.getTableAssociation().setSelectable(true);
		impl.getTableAssociation().setImmediate(true);
		impl.getTableAssociation().setReadThrough(true);
		impl.getTableAssociation().setMultiSelect(true);
		impl.getTableAssociation().setNullSelectionAllowed(true);
		impl.getTableAssociation().addContainerProperty(USCUAssociationListView.TABLE_COLUMN_WU_ID, String.class, null);
		impl.getTableAssociation().addContainerProperty(USCUAssociationListView.TABLE_COLUMN_WU_TYPE, String.class, null);
		impl.getTableAssociation().addContainerProperty(USCUAssociationListView.TABLE_COLUMN_WU_STATE, String.class, null);

		impl.getButtonUpdateList().addListener(new ClickListener() {
			public void buttonClick(ClickEvent event) {
				notifyEventoToControllerWithData(USCUAssociationListView.EVENT_UPDATE_LIST, event.getSource(), null);
			}
		});
		impl.getButtonLoadInstance().addListener(new ClickListener() {
			public void buttonClick(ClickEvent event) {
				notifyEventoToControllerWithData(USCUAssociationListView.EVENT_LOAD_INSTANCE, event.getSource(), impl.getTableAssociation().getValue());
			}
		});
		impl.getButtonRemoveAssociation().addListener(new ClickListener() {
			public void buttonClick(ClickEvent event) {
				if (impl.getTableAssociation().getValue() == null || ((Set<UnitServerCuInstance>) impl.getTableAssociation().getValue()).size() == 0) {
					RefVaadinErrorDialog.shorError(getWindow(), "Remove Association Error", "No association selected");
					return;
				}
				notifyEventoToControllerWithData(USCUAssociationListView.EVENT_REMOVE_ASSOCIATION, event.getSource(), impl.getTableAssociation().getValue());
			}
		});
		impl.getButtonUnloadInstance().addListener(new ClickListener() {
			public void buttonClick(ClickEvent event) {
				notifyEventoToControllerWithData(USCUAssociationListView.EVENT_UNLOAD_INSTANCE, event.getSource(), impl.getTableAssociation().getValue());
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
			if (viewEvent.getEventKind().equals(USCUAssociationListView.SET_US_ALIAS)) {
				impl.getUsSelected().setValue(viewEvent.getEventData().toString());
			} else if (viewEvent.getEventKind().equals(USCUAssociationListView.EVENT_UPDATE_LIST)) {
				impl.getTableAssociation().removeAllItems();
				currentInstancesForUnitServer = (Vector<UnitServerCuInstance>) viewEvent.getEventData();
				if (currentInstancesForUnitServer == null)
					return;

				for (UnitServerCuInstance unitServerCuInstance : currentInstancesForUnitServer) {
					Item woItem = impl.getTableAssociation().addItem(unitServerCuInstance);
					woItem.getItemProperty(USCUAssociationListView.TABLE_COLUMN_WU_ID).setValue(unitServerCuInstance.getCuId());
					woItem.getItemProperty(USCUAssociationListView.TABLE_COLUMN_WU_TYPE).setValue(unitServerCuInstance.getCuType());
					woItem.getItemProperty(USCUAssociationListView.TABLE_COLUMN_WU_STATE).setValue(unitServerCuInstance.getState());
				}

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

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.vaadin.event.ItemClickEvent.ItemClickListener#itemClick(com.vaadin.event.ItemClickEvent)
	 */
	public void itemClick(ItemClickEvent event) {
		if (event.isDoubleClick() && event.getSource() instanceof Table) {
			((Table) event.getSource()).select(event.getItemId());
		}
	}
}
