package it.infn.chaos.mds;

import java.util.Date;
import java.util.Observable;

import org.ref.common.mvc.ViewNotifyEvent;
import org.ref.server.webapp.RefVaadinBasePanel;

import com.vaadin.event.Action;
import com.vaadin.event.ItemClickEvent;
import com.vaadin.event.ItemClickEvent.ItemClickListener;
import com.vaadin.ui.AbstractSelect.ItemDescriptionGenerator;
import com.vaadin.ui.Button.ClickEvent;
import com.vaadin.ui.Button.ClickListener;
import com.vaadin.ui.Component;
import com.vaadin.ui.Label;
import com.vaadin.ui.Table;
import com.vaadin.ui.Window;

@SuppressWarnings("serial")
public class MDSAppView extends RefVaadinBasePanel implements ItemClickListener {
	static final Action			ACTION_EDIT									= new Action("Edit");
	static final Action			ACTION_SAVE									= new Action("Save");

	static final Action			ACTION_US_EDIT_ALIAS						= new Action("Edit Unit Server Alias");
	static final Action			ACTION_US_LOAD_ALL							= new Action("Load all work unit");
	static final Action			ACTION_US_UNLOAD_ALL						= new Action("Unload all work unit");
	static final Action			ACTION_US_SHOW_ALL							= new Action("Show work unit");

	static final Action[]		ACTIONS_TO_EDIT								= new Action[] { ACTION_EDIT };
	static final Action[]		ACTIONS_IN_EDIT								= new Action[] { ACTION_SAVE };
	static final Action[]		EDIT_US										= new Action[] { ACTION_US_EDIT_ALIAS, ACTION_US_LOAD_ALL, ACTION_US_UNLOAD_ALL, ACTION_US_SHOW_ALL };
	
	public static final String	EVENT_DEVICE_SELECTED						= "MDSAppView_EVENT_DEVICE_SELECTED";
	public static final String	EVENT_DATASET_SELECTED						= "MDSAppView_EVENT_DATASET_SELECTED";
	public static final String	EVENT_ATTRIBUTE_EDITING						= "MDSAppView_EVENT_ATTRIBUTE_EDITING";
	public static final String	EVENT_ATTRIBUTE_SAVE						= "MDSAppView_EVENT_ATTRIBUTE_SAVE";
	public static final String	EVENT_DELETE_SELECTED_DEVICE				= "MDSAppView_EVENT_DELETE_SELECTED_DEVICE";
	public static final String	EVENT_UPDATE_DEVICE_LIST					= "MDSAppView_EVENT_UPDATE_DEVICE_LIST";
	public static final String	EVENT_UPDATE_DS_ATTRIBUTE_LIST				= "MDSAppView_EVENT_UPDATE_DS_ATTRIBUTE_LIST";
	public static final String	EVENT_SHOW_PREFERENCE						= "MDSAppView_EVENT_SHOW_PREFERENCE";
	public static final String	EVENT_INITIALIZE_DEVICE_AT_STARTUP			= "MDSAppView_EVENT_INITIALIZE_DEVICE_AT_STARTUP";
	public static final String	EVENT_UPDATE_UNIT_SERVER_LIST				= "MDSAppView_EVENT_UPDATE_UNIT_SERVER_LIST";

	public static final String	EVENT_UNIT_SERVER_SELECTED					= "MDSAppView_EVENT_UNIT_SERVER_SELECTED";
	public static final String	EVENT_UNIT_SERVER_CU_TYPE_SELECTED			= "MDSAppView_EVENT_UNIT_SERVER_CU_TYPE_SELECTED";
	public static final String	EVENT_UNIT_SERVER_CREATE_US_CU_ASSOCIATION	= "MDSAppView_EVENT_UNIT_SERVER_CREATE_US_CU_ASSOCIATION";
	public static final String	EVENT_UNIT_SERVER_LOAD_ALL_ASSOCIATION		= "MDSAppView_EVENT_UNIT_SERVER_LOAD_ALL_ASSOCIATION";
	public static final String	EVENT_UNIT_SERVER_UNLOAD_ALL_ASSOCIATION	= "MDSAppView_EVENT_UNIT_SERVER_UNLOAD_ALL_ASSOCIATION";
	public static final String	EVENT_UNIT_SERVER_SHOW_ALL_ASSOCIATION		= "MDSAppView_EVENT_UNIT_SERVER_SHOW_ALL_ASSOCIATION";
	public static final String	EVENT_UNIT_SERVER_NEW						= "MDSAppView_EVENT_UNIT_SERVER_NEW";
	public static final String	EVENT_UNIT_SERVER_DELETE					= "MDSAppView_EVENT_UNIT_SERVER_DELETE";
	public static final String	EVENT_UNIT_SERVER_EDIT_ALIAS				= "MDSAppView_EVENT_UNIT_SERVER_EDIT_ALIAS";
	public static final String	EVENT_UNIT_SERVER_CU_TYPE_ADD				= "MDSAppView_EVENT_UNIT_SERVER_CU_TYPE_ADD";
	public static final String	EVENT_UNIT_SERVER_CU_TYPE_REMOVE			= "MDSAppView_EVENT_UNIT_SERVER_CU_TYPE_REMOVE";
	public static final String	EVENT_UNIT_SERVER_CU_TYPE_UPDATE_LIST		= "MDSAppView_EVENT_UNIT_SERVER_CU_TYPE_UPDATE_LIST";

	public static final String	TAB1_CU_PARENT								= "CU Parent";
	public static final String	TAB1_DEVICE_INSTANCE						= "Device Instance";
	public static final String	TAB1_NET_ADDRESS							= "Net Address";
	public static final String	TAB1_LAST_HB								= "Last HB";
	public static final String	TAB2_TIMESTAMP								= "Date Time";
	public static final String	TAB2_ATTR_NUMBER							= "Attributes";
	public static final String	TAB3_NAME									= "Name";
	public static final String	TAB3_TYPE									= "Type";
	public static final String	TAB3_DIR									= "Direction";
	public static final String	TAB3_RANG_MIN								= "Min Range";
	public static final String	TAB3_RANG_MAX								= "Max Range";
	public static final String	TAB3_DEF_VALUE								= "Default";
	public static final String	TAB3_TAGS_PATH								= "Tag Path";
	public static final String	KEY_USERVER_TAB								= "KEY_USERVER_TAB";
	public static final String	TAB_UNIT_SERVER_NAME						= "Unit Server Name";
	public static final String	TAB_UNIT_SERVER_ADDRESS						= "Address";
	public static final String	TAB_UNIT_SERVER_HB_TS						= "heartbeat timestamp";
	public static final String	KEY_USERVER_CUTYPE_TAB						= "KEY_USERVER_CUTYPE_TAB";
	public static final String	TAB_UNIT_SERVER_CUTYPE_TPE_NAME				= "Type Name";
	public static final String	KEY_DEVICE_TAB								= "DEVICE_TAB";
	public static final String	KEY_DATASET_TAB								= "KEY_DATASET_TAB";
	public static final String	KEY_DEVICE_START_AT_INIT_BUTTON				= "KEY_DEVICE_START_AT_INIT_BUTTON";
	public static final String	KEY_DATASET_ATTRIBUTE_TAB					= "KEY_DATASET_ATTRIBUTE_TAB";

	private MainView			mv											= new MainView();
	private boolean				editingAttribute							= false;

	@Override
	public void initGui() {
		addComponent(mv);
		mv.setWidth("100.0%");
		mv.setHeight("100.0%");

		setComponentKey(KEY_USERVER_TAB, mv.getTableUnitServer());
		setComponentKey(KEY_USERVER_CUTYPE_TAB, mv.getTableUnitServerCUType());

		setComponentKey(KEY_DEVICE_TAB, mv.getTableDevice());
		setComponentKey(KEY_DEVICE_START_AT_INIT_BUTTON, mv.getButtonInitializedAtStartup());
		mv.getTableUnitServer().addListener(this);
		mv.getTableUnitServer().setEditable(false);
		mv.getTableUnitServer().setSelectable(true);
		// mv.getTableUnitServer().setImmediate(true);
		// mv.getTableUnitServer().setReadThrough(true);
		mv.getTableUnitServer().addContainerProperty(TAB_UNIT_SERVER_NAME, String.class, null);
		mv.getTableUnitServer().addContainerProperty(TAB_UNIT_SERVER_ADDRESS, String.class, null);
		mv.getTableUnitServer().addContainerProperty(TAB_UNIT_SERVER_HB_TS, Date.class, null);
		mv.getTableUnitServer().addActionHandler(new Action.Handler() {
			public Action[] getActions(Object target, Object sender) {
				return EDIT_US;
			}

			public void handleAction(Action action, Object sender, Object target) {
				setEditingAttribute(!isEditingAttribute());
				if (ACTION_US_EDIT_ALIAS == action) {
					notifyEventoToControllerWithData(EVENT_UNIT_SERVER_EDIT_ALIAS, mv.getTableUnitServer().getValue(), sender);
				}else if (ACTION_US_LOAD_ALL == action) {
					notifyEventoToControllerWithData(EVENT_UNIT_SERVER_LOAD_ALL_ASSOCIATION, null, null);
				}else if (ACTION_US_UNLOAD_ALL == action) {
					notifyEventoToControllerWithData(EVENT_UNIT_SERVER_UNLOAD_ALL_ASSOCIATION, null, sender);
				}else if (ACTION_US_SHOW_ALL == action) {
					notifyEventoToControllerWithData(EVENT_UNIT_SERVER_SHOW_ALL_ASSOCIATION, null, sender);
				}
			}

		});

		mv.getTableUnitServerCUType().addListener(this);
		mv.getTableUnitServerCUType().setEditable(false);
		mv.getTableUnitServerCUType().setSelectable(true);
		mv.getTableUnitServerCUType().setImmediate(true);
		mv.getTableUnitServerCUType().setReadThrough(true);
		mv.getTableUnitServerCUType().addContainerProperty(TAB_UNIT_SERVER_CUTYPE_TPE_NAME, String.class, null);

		mv.getTableDevice().addListener(this);
		mv.getTableDevice().setEditable(false);
		mv.getTableDevice().setSelectable(true);
		mv.getTableDevice().setImmediate(true);
		mv.getTableDevice().setReadThrough(true);
		mv.getTableDevice().setNullSelectionAllowed(false);
		mv.getTableDevice().addContainerProperty(TAB1_DEVICE_INSTANCE, String.class, null);
		mv.getTableDevice().addContainerProperty(TAB1_CU_PARENT, String.class, null);
		mv.getTableDevice().addContainerProperty(TAB1_NET_ADDRESS, String.class, null);
		mv.getTableDevice().addContainerProperty(TAB1_LAST_HB, Date.class, null);
		mv.getTableDevice().addContainerProperty("INIT", String.class, null);

		mv.getButtonNewUS().addListener(new ClickListener() {
			public void buttonClick(ClickEvent event) {
				notifyEventoToControllerWithData(EVENT_UNIT_SERVER_NEW, event.getSource(), null);
			}
		});

		mv.getButtonDelUS().addListener(new ClickListener() {
			public void buttonClick(ClickEvent event) {
				notifyEventoToControllerWithData(EVENT_UNIT_SERVER_DELETE, event.getSource(), null);
			}
		});

		mv.getButtonUnitServerUpdate().addListener(new ClickListener() {
			public void buttonClick(ClickEvent event) {
				notifyEventoToControllerWithData(EVENT_UPDATE_UNIT_SERVER_LIST, event.getSource(), null);
			}
		});

		mv.getButtonUSCUAssociate().addListener(new ClickListener() {
			public void buttonClick(ClickEvent event) {
				notifyEventoToControllerWithData(EVENT_UNIT_SERVER_CREATE_US_CU_ASSOCIATION, event.getSource(), null);
			}
		});

		mv.getButtonRefresh().addListener(new ClickListener() {
			public void buttonClick(ClickEvent event) {
				notifyEventoToControllerWithData(EVENT_UPDATE_DEVICE_LIST, event.getSource(), null);
			}
		});

		mv.getButtonDelete().addListener(new ClickListener() {
			public void buttonClick(ClickEvent event) {
				notifyEventoToControllerWithData(EVENT_DELETE_SELECTED_DEVICE, event.getSource(), null);
			}
		});

		mv.getPreferenceButton().addListener(new ClickListener() {
			public void buttonClick(ClickEvent event) {
				notifyEventoToControllerWithData(EVENT_SHOW_PREFERENCE, event.getSource(), null);
			}
		});

		mv.getButtonInitializedAtStartup().addListener(new ClickListener() {
			public void buttonClick(ClickEvent event) {
				notifyEventoToControllerWithData(EVENT_INITIALIZE_DEVICE_AT_STARTUP, event.getSource(), null);
			}
		});

		mv.getButtonAddCUType().addListener(new ClickListener() {
			public void buttonClick(ClickEvent event) {
				if (mv.getTableUnitServer().getValue() == null) {
					getWindow().showNotification("New CU Type", "<b>A unit server needs to be selected</b>", Window.Notification.TYPE_ERROR_MESSAGE);
				} else {
					notifyEventoToControllerWithData(EVENT_UNIT_SERVER_CU_TYPE_ADD, event.getSource(), mv.getTableUnitServer().getValue());
				}
			}
		});
		mv.getButtonRemoveCUType().addListener(new ClickListener() {
			public void buttonClick(ClickEvent event) {
				if (mv.getTableUnitServerCUType().getValue() == null) {
					getWindow().showNotification("Delete CU Type", "<b>A cu type needs to be selected</b>", Window.Notification.TYPE_ERROR_MESSAGE);
				} else {
					notifyEventoToControllerWithData(EVENT_UNIT_SERVER_CU_TYPE_REMOVE, event.getSource(), mv.getTableUnitServerCUType().getValue());
				}
			}
		});
		mv.getButtonUpdateListCUType().addListener(new ClickListener() {
			public void buttonClick(ClickEvent event) {
				if (mv.getTableUnitServer().getValue() == null) {
					getWindow().showNotification("Update CU Type List", "<b>A unit server needs to be seletec</b>", Window.Notification.TYPE_ERROR_MESSAGE);
				} else {
					notifyEventoToControllerWithData(EVENT_UNIT_SERVER_CU_TYPE_UPDATE_LIST, event.getSource(), mv.getTableUnitServer().getValue());
				}
			}
		});

		mv.getTableDataset().addListener(this);
		setComponentKey(KEY_DATASET_TAB, mv.getTableDataset());

		mv.getTableDataset().addContainerProperty(TAB2_TIMESTAMP, Date.class, null);
		mv.getTableDataset().addContainerProperty(TAB2_ATTR_NUMBER, Integer.class, null);
		mv.getTableDataset().setNullSelectionAllowed(false);
		mv.getTableDataset().setSelectable(true);
		mv.getTableDataset().setImmediate(true);
		mv.getButtonRefresh().addListener(new ClickListener() {
			public void buttonClick(ClickEvent event) {
				notifyEventoToControllerWithData(EVENT_UPDATE_DS_ATTRIBUTE_LIST, event.getSource(), null);
			}
		});

		mv.getTableDatasetAttribute().addListener(this);
		setComponentKey(KEY_DATASET_ATTRIBUTE_TAB, mv.getTableDatasetAttribute());

		mv.getTableDatasetAttribute().addContainerProperty(TAB3_NAME, Label.class, null);
		mv.getTableDatasetAttribute().addContainerProperty(TAB3_TYPE, Label.class, null);
		mv.getTableDatasetAttribute().addContainerProperty(TAB3_DIR, Label.class, null);
		mv.getTableDatasetAttribute().addContainerProperty(TAB3_RANG_MIN, String.class, null);
		mv.getTableDatasetAttribute().addContainerProperty(TAB3_RANG_MAX, String.class, null);
		mv.getTableDatasetAttribute().addContainerProperty(TAB3_DEF_VALUE, String.class, null);
		mv.getTableDatasetAttribute().addContainerProperty(TAB3_TAGS_PATH, String.class, null);
		mv.getTableDatasetAttribute().addActionHandler(new Action.Handler() {
			public Action[] getActions(Object target, Object sender) {
				return isEditingAttribute() ? ACTIONS_IN_EDIT : ACTIONS_TO_EDIT;
			}

			public void handleAction(Action action, Object sender, Object target) {
				setEditingAttribute(!isEditingAttribute());
				if (ACTION_EDIT == action) {
					mv.getTableDatasetAttribute().setEditable(!mv.getTableDatasetAttribute().isEditable());
					mv.getTableDatasetAttribute().refreshRowCache();
				} else if (ACTION_SAVE == action) {
					mv.getTableDatasetAttribute().setEditable(!mv.getTableDatasetAttribute().isEditable());
					mv.getTableDatasetAttribute().refreshRowCache();
					notifyEventoToControllerWithData(EVENT_ATTRIBUTE_SAVE, null, null);

				}
			}

		});
		mv.getTableDatasetAttribute().setSelectable(true);
		mv.getTableDatasetAttribute().setImmediate(true);
		mv.getTableDatasetAttribute().setItemDescriptionGenerator(new ItemDescriptionGenerator() {
			public String generateDescription(Component source, Object itemId, Object propertyId) {
				if (itemId != null)
					return itemId.toString();
				else
					return null;
			}
		});
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see org.ref.server.webapp.RefVaadinBasePanel#update(java.util.Observable, java.lang.Object)
	 */
	public void update(Observable source, Object sourceData) {
		if (sourceData instanceof ViewNotifyEvent) {
			ViewNotifyEvent viewEvent = (ViewNotifyEvent) sourceData;
			if (viewEvent.getEventKind().equals(MDSAppView.EVENT_ATTRIBUTE_EDITING)) {
				setEditingAttribute(true);
				mv.getTableDatasetAttribute().setEditable(true);
				mv.getTableDatasetAttribute().refreshRowCache();
			}
		}
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.vaadin.event.ItemClickEvent.ItemClickListener#itemClick(com.vaadin.event.ItemClickEvent)
	 */
	public void itemClick(ItemClickEvent event) {
		boolean modifier = event.isCtrlKey() || event.isShiftKey() || event.isMetaKey() || event.isAltKey();
		if (event.getSource().equals(mv.getButtonUSCULoadAll())) {
			notifyEventoToControllerWithData(EVENT_UNIT_SERVER_LOAD_ALL_ASSOCIATION, event.getSource(), event.getItemId());
		} else if (event.getSource().equals(mv.getButtonUSCUUnloadAll())) {
			notifyEventoToControllerWithData(EVENT_UNIT_SERVER_UNLOAD_ALL_ASSOCIATION, event.getSource(), event.getItemId());
		} else if (event.getSource().equals(mv.getButtonUSCUShowAll())) {
			notifyEventoToControllerWithData(EVENT_UNIT_SERVER_SHOW_ALL_ASSOCIATION, event.getSource(), event.getItemId());
		} else if (event.getSource().equals(mv.getTableUnitServer())) {
			notifyEventoToControllerWithData(EVENT_UNIT_SERVER_SELECTED, event.getSource(), event.getItemId());
		} else if (event.getSource().equals(mv.getTableUnitServerCUType())) {
			notifyEventoToControllerWithData(EVENT_UNIT_SERVER_CU_TYPE_SELECTED, event.getSource(), event.getItemId());
		} else if (event.getSource().equals(mv.getTableDevice())) {
			notifyEventoToControllerWithData(EVENT_DEVICE_SELECTED, event.getSource(), event.getItemId());
		} else if (event.getSource().equals(mv.getTableDataset())) {
			notifyEventoToControllerWithData(EVENT_DATASET_SELECTED, event.getSource(), event.getItemId());
		} else if (event.getSource().equals(mv.getTableDatasetAttribute())) {
			if (event.isDoubleClick()) {
				notifyEventoToControllerWithData(EVENT_ATTRIBUTE_EDITING, event.getSource(), event.getItemId());
			}
		}
	}

	/**
	 * @return the editingAttribute
	 */
	public boolean isEditingAttribute() {
		return editingAttribute;
	}

	/**
	 * @param editingAttribute
	 *            the editingAttribute to set
	 */
	public void setEditingAttribute(boolean editingAttribute) {
		this.editingAttribute = editingAttribute;
	}
}
