/**
 * 
 */
package it.infn.chaos.mds;

import it.infn.chaos.mds.business.DatasetAttribute;
import it.infn.chaos.mds.business.UnitServerCuInstance;

import java.util.Collection;
import java.util.Observable;
import java.util.Set;
import java.util.Vector;

import org.ref.common.mvc.ViewNotifyEvent;
import org.ref.server.webapp.RefVaadinBasePanel;
import org.ref.server.webapp.dialog.RefVaadinErrorDialog;

import com.vaadin.data.Item;
import com.vaadin.event.Action;
import com.vaadin.event.ItemClickEvent;
import com.vaadin.event.ItemClickEvent.ItemClickListener;
import com.vaadin.ui.Button.ClickEvent;
import com.vaadin.ui.Button.ClickListener;
import com.vaadin.ui.Window;
import com.vaadin.ui.Window.Notification;

/**
 * @author bisegni
 * 
 */
public class USCUAssociationListView extends RefVaadinBasePanel implements com.vaadin.data.Container.PropertySetChangeListener, ItemClickListener {
	private static final String				ATTRIBUTE_DEFAULT_VALUE			= "Default Value";
	private static final String				ATTRIBUTE_MIN_VALUE				= "Min Value";
	private static final String				ATTRIBUTE_MAX_VALUE				= "Max Value";
	private static final String				ATTRIBUTE_NAME					= "Attribute name";
	private static final long				serialVersionUID				= -841710385419733478L;
	static final Action						ACTION_EDIT						= new Action("Edit");
	static final Action						ACTION_SAVE						= new Action("Save");
	static final Action[]					ACTIONS_TO_EDIT					= new Action[] { ACTION_EDIT };
	static final Action[]					ACTIONS_IN_EDIT					= new Action[] { ACTION_SAVE };
	public static final String				SET_US_ALIAS					= "SET_US_ALIAS";
	public static final String				EVENT_REMOVE_ASSOCIATION		= "USCUAssociationListView_EVENT_REMOVE_ASSOCIATION";
	public static final String				EVENT_LOAD_INSTANCE				= "USCUAssociationListView_EVENT_LOAD_INSTANCE";
	public static final String				EVENT_UNLOAD_INSTANCE			= "USCUAssociationListView_EVENT_UNLOAD_INSTANCE";
	public static final String				EVENT_UPDATE_LIST				= "USCUAssociationListView_EVENT_UPDATE_LIST";
	public static final String				EVENT_SAVE_ATTRIBUTE_CONFIG		= "USCUAssociationListView_EVENT_SAVE_ATTRIBUTE_CONFIG";
	public static final String				EVENT_LOAD_INSTANCE_ATTRIBUTE	= "USCUAssociationListView_EVENT_LOAD_INSTANCE_ATTRIBUTE";
	public static final String				EVENT_SWITCH_AUTOLOAD			= "USCUAssociationListView_EVENT_SWITCH_AUTOLOAD";
	public static final String				EVENT_CLOSE_VIEW				= "USCUAssociationListView_EVENT_CLOSE_VIEW";
	public static final String				EVENT_EDIT_ASSOCIATION			= "USCUAssociationListView_EVENT_EDIT_ASSOCIATION";
	private static final Object				TABLE_COLUMN_WU_ID				= "Unique ID";
	private static final Object				TABLE_COLUMN_WU_TYPE			= "type";
	private static final Object				TABLE_COLUMN_WU_AUTOLOAD		= "autoload";
	private static final Object				TABLE_COLUMN_WU_STATE			= "State";
	private USCUAssociationListVaadin		impl							= new USCUAssociationListVaadin();
	private boolean							editingServer					= false;
	private Vector<UnitServerCuInstance>	currentInstancesForUnitServer	= null;
	protected int							idx;

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
		// impl.getTableAssociation().setImmediate(true);
		// impl.getTableAssociation().setReadThrough(true);
		impl.getTableAssociation().setMultiSelect(true);
		impl.getTableAssociation().setNullSelectionAllowed(true);
		impl.getTableAssociation().addContainerProperty(USCUAssociationListView.TABLE_COLUMN_WU_ID, String.class, null);
		impl.getTableAssociation().addContainerProperty(USCUAssociationListView.TABLE_COLUMN_WU_TYPE, String.class, null);
		impl.getTableAssociation().addContainerProperty(USCUAssociationListView.TABLE_COLUMN_WU_AUTOLOAD, String.class, null);
		impl.getTableAssociation().addContainerProperty(USCUAssociationListView.TABLE_COLUMN_WU_STATE, String.class, null);
		impl.getTableAssociation().addActionHandler(new Action.Handler() {
			public Action[] getActions(Object target, Object sender) {
				return new Action[] { ACTION_EDIT };
			}

			public void handleAction(Action action, Object sender, Object target) {
				if (ACTION_EDIT == action) {
					if (impl.getTableAssociation().getValue() == null || ((Set<UnitServerCuInstance>) impl.getTableAssociation().getValue()).size() != 1) {
						getWindow().showNotification("Edit association error", "Only one association need to be selected", Notification.TYPE_ERROR_MESSAGE);
						return;
					} else {
						notifyEventoToControllerWithData(USCUAssociationListView.EVENT_EDIT_ASSOCIATION, sender, impl.getTableAssociation().getValue());
					}
				}
			}

		});
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
					getWindow().showNotification("Remove Association Error", "No association selected", Notification.TYPE_ERROR_MESSAGE);
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
		impl.getButtonSwapAutoLoad().addListener(new ClickListener() {
			public void buttonClick(ClickEvent event) {
				notifyEventoToControllerWithData(USCUAssociationListView.EVENT_SWITCH_AUTOLOAD, event.getSource(), impl.getTableAssociation().getValue());
			}
		});
		impl.getButtonCloseView().addListener(new ClickListener() {
			public void buttonClick(ClickEvent event) {
				notifyEventoToControllerWithData(USCUAssociationListView.EVENT_CLOSE_VIEW, event.getSource(), impl.getTableAssociation().getValue());
			}
		});
		// ----------attribute config
		impl.getTableAttributeConfig().addListener((ItemClickListener) this);
		impl.getTableAttributeConfig().setSelectable(true);
		impl.getTableAttributeConfig().setImmediate(true);
		impl.getTableAttributeConfig().setReadThrough(true);
		impl.getTableAttributeConfig().setMultiSelect(true);
		impl.getTableAttributeConfig().setNullSelectionAllowed(true);
		impl.getTableAttributeConfig().addContainerProperty(ATTRIBUTE_NAME, String.class, null);
		impl.getTableAttributeConfig().addContainerProperty(ATTRIBUTE_MAX_VALUE, String.class, null);
		impl.getTableAttributeConfig().addContainerProperty(ATTRIBUTE_MIN_VALUE, String.class, null);
		impl.getTableAttributeConfig().addContainerProperty(ATTRIBUTE_DEFAULT_VALUE, String.class, null);
		impl.getTableAttributeConfig().addActionHandler(new Action.Handler() {
			public Action[] getActions(Object target, Object sender) {
				return impl.getTableAttributeConfig().isEditable() ? ACTIONS_IN_EDIT : ACTIONS_TO_EDIT;
			}

			public void handleAction(Action action, Object sender, Object target) {
				impl.getTableAttributeConfig().setEditable(!impl.getTableAttributeConfig().isEditable());
				if (ACTION_EDIT == action) {
					impl.getTableAttributeConfig().refreshRowCache();
				} else if (ACTION_SAVE == action) {
					impl.getTableAttributeConfig().refreshRowCache();
				}
			}

		});
		impl.getButtonAddAttributeConfig().addListener(new ClickListener() {
			@SuppressWarnings("unchecked")
			public void buttonClick(ClickEvent event) {
				if (((Set<UnitServerCuInstance>) impl.getTableAssociation().getValue()).size() != 1) {
					getWindow().showNotification("Attribute value Error", "<b>An association need to be selected</b>", Window.Notification.TYPE_ERROR_MESSAGE);
				} else {
					Item item = impl.getTableAttributeConfig().addItem(new Integer(idx++));
					item.getItemProperty(ATTRIBUTE_NAME).setValue(ATTRIBUTE_NAME);
					item.getItemProperty(ATTRIBUTE_MAX_VALUE).setValue(ATTRIBUTE_MAX_VALUE);
					item.getItemProperty(ATTRIBUTE_MIN_VALUE).setValue(ATTRIBUTE_MIN_VALUE);
					item.getItemProperty(ATTRIBUTE_DEFAULT_VALUE).setValue(ATTRIBUTE_DEFAULT_VALUE);
				}
			}
		});

		impl.getButtonRemoveAttributeConfig().addListener(new ClickListener() {
			public void buttonClick(ClickEvent event) {
				Set<Integer> c = (Set<Integer>) impl.getTableAttributeConfig().getValue();
				for (Integer idx : c) {
					impl.getTableAttributeConfig().removeItem(idx);
				}
			}
		});

		impl.getButtonSaveAttributeConfig().addListener(new ClickListener() {
			public void buttonClick(ClickEvent event) {
				// filel the object
				UnitServerCuInstance selectedInstance = null;
				Set<UnitServerCuInstance> selected = (Set<UnitServerCuInstance>) impl.getTableAssociation().getValue();
				if (selected.size() != 1) {
					getWindow().showNotification("Save Attribute Configuration Error", "<b>Only one association need to be selected</b>", Window.Notification.TYPE_ERROR_MESSAGE);
					return;
				}
				for (UnitServerCuInstance unitServerCuInstance : selected) {
					selectedInstance = unitServerCuInstance;
					break;
				}

				Vector<DatasetAttribute> toSave = new Vector<DatasetAttribute>();
				Collection<Integer> indexList = (Collection<Integer>) impl.getTableAttributeConfig().getItemIds();
				for (Integer idx : indexList) {
					Double def=null,max=null,min=null;
					String sname = impl.getTableAttributeConfig().getItem(idx).getItemProperty(ATTRIBUTE_NAME).toString();
					String smax = impl.getTableAttributeConfig().getItem(idx).getItemProperty(ATTRIBUTE_MAX_VALUE).toString();
					String smin = impl.getTableAttributeConfig().getItem(idx).getItemProperty(ATTRIBUTE_MIN_VALUE).toString();
					String sdef =impl.getTableAttributeConfig().getItem(idx).getItemProperty(ATTRIBUTE_DEFAULT_VALUE).toString();
					
					DatasetAttribute da = new DatasetAttribute();
					
					if(sname.isEmpty()){
						getWindow().showNotification("Save Attribute Configuration Error", "<b>Attribute Name cannot be empty</b>", Window.Notification.TYPE_ERROR_MESSAGE);
						return;
					}
					
					da.setName(sname);
					if(!smax.isEmpty()){
						try {
							max = Double.parseDouble(smax);
						} catch(Throwable e) {
							getWindow().showNotification("Save Attribute Configuration Error", "<b>Attribute \""+sname+"\" MAX \""+smax+"\" bad format</b>", Window.Notification.TYPE_ERROR_MESSAGE);
							return;
						}
					}
					if(!smin.isEmpty()){
						try {
							min = Double.parseDouble(smin);
						} catch(Throwable e) {
							getWindow().showNotification("Save Attribute Configuration Error", "<b>Attribute \""+sname+"\" MIN \"" +smin+"\" bad format</b>", Window.Notification.TYPE_ERROR_MESSAGE);
							return;
						}
					}
					
					if(!sdef.isEmpty()){
						try {
							def = Double.parseDouble(sdef);
						} catch(Throwable e) {
							getWindow().showNotification("Save Attribute Configuration Error", "<b>Attribute \""+sname+"\" DEFAULT \""+sdef+"\" bad format</b>", Window.Notification.TYPE_ERROR_MESSAGE);
							return;
						}
						
						if(min!=null && def<min){
							getWindow().showNotification("Save Attribute Configuration Error", "<b>Attribute \""+sname+"\" DEFAULT "+sdef+" cannot be lower than MIN "+smin+"</b>", Window.Notification.TYPE_ERROR_MESSAGE);
							return;
							
						}
						if(max!=null && def>max){
							getWindow().showNotification("Save Attribute Configuration Error", "<b>Attribute \""+sname+"\" DEFAULT "+sdef+" cannot be greater than MAX "+smax+"</b>", Window.Notification.TYPE_ERROR_MESSAGE);
							return;
							
						}
					}
					
					da.setDefaultValueNoCheck(sdef);
					
					da.setRangeMaxNoCheck(smax);
					da.setRangeMinNoCheck(smin);
					toSave.addElement(da);
				}
				selectedInstance.setAttributeConfigutaion(toSave);
				notifyEventoToControllerWithData(USCUAssociationListView.EVENT_SAVE_ATTRIBUTE_CONFIG, event.getSource(), selectedInstance);
				getWindow().showNotification("Attribute Configuration", "<b>The configuraiton has been saved</b>", Window.Notification.TYPE_HUMANIZED_MESSAGE);
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
				impl.getTableAttributeConfig().removeAllItems();
				currentInstancesForUnitServer = (Vector<UnitServerCuInstance>) viewEvent.getEventData();
				if (currentInstancesForUnitServer == null)
					return;

				for (UnitServerCuInstance unitServerCuInstance : currentInstancesForUnitServer) {
					Item woItem = impl.getTableAssociation().addItem(unitServerCuInstance);
					woItem.getItemProperty(USCUAssociationListView.TABLE_COLUMN_WU_ID).setValue(unitServerCuInstance.getCuId());
					woItem.getItemProperty(USCUAssociationListView.TABLE_COLUMN_WU_TYPE).setValue(unitServerCuInstance.getCuType());
					woItem.getItemProperty(USCUAssociationListView.TABLE_COLUMN_WU_AUTOLOAD).setValue(unitServerCuInstance.getAutoLoad());
					woItem.getItemProperty(USCUAssociationListView.TABLE_COLUMN_WU_STATE).setValue(unitServerCuInstance.getState());
				}

			} else if (viewEvent.getEventKind().equals(USCUAssociationListView.EVENT_LOAD_INSTANCE_ATTRIBUTE)) {
				impl.getTableAttributeConfig().removeAllItems();
				Vector<DatasetAttribute> datattr = (Vector<DatasetAttribute>) viewEvent.getEventData();
				if (datattr == null)
					return;

				for (DatasetAttribute da : datattr) {
					Item woItem = impl.getTableAttributeConfig().addItem(new Integer(idx++));
					woItem.getItemProperty(USCUAssociationListView.ATTRIBUTE_NAME).setValue(da.getName());
					woItem.getItemProperty(USCUAssociationListView.ATTRIBUTE_DEFAULT_VALUE).setValue(da.getDefaultValue());
					woItem.getItemProperty(USCUAssociationListView.ATTRIBUTE_MAX_VALUE).setValue(da.getRangeMax());
					woItem.getItemProperty(USCUAssociationListView.ATTRIBUTE_MIN_VALUE).setValue(da.getRangeMin());
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
		if (event.getSource().equals(impl.getTableAssociation())) {
			associationSeleteionHasChanged(event.getItemId());
		}
	}

	/**
	 * 
	 * @param itemID
	 */
	private void associationSeleteionHasChanged(Object itemID) {
		impl.getTableAttributeConfig().removeAllItems();
		UnitServerCuInstance association = (UnitServerCuInstance) itemID;
		if (association == null)
			return;
		notifyEventoToControllerWithData(EVENT_LOAD_INSTANCE_ATTRIBUTE, impl.getTableAssociation(), association);
	}
}
