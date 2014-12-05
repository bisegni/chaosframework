package it.infn.chaos.mds;

import it.infn.chaos.mds.business.Device;
import it.infn.chaos.mds.business.UnitServer;
import it.infn.chaos.mds.business.UnitServerCuInstance;
import it.infn.chaos.mds.event.ChaosEventComponent;
import it.infn.chaos.mds.event.ChaosEventComponent.ChaosEventListener;
import it.infn.chaos.mds.event.VaadinEvent;

import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.HashSet;
import java.util.List;
import java.util.Locale;
import java.util.Observable;
import java.util.Set;
import java.util.Vector;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.ref.common.mvc.ViewNotifyEvent;
import org.ref.server.webapp.RefVaadinBasePanel;
import org.vaadin.dialogs.ConfirmDialog;

import com.github.wolfie.refresher.Refresher;
import com.vaadin.data.Item;
import com.vaadin.event.Action;
import com.vaadin.event.ItemClickEvent;
import com.vaadin.event.ItemClickEvent.ItemClickListener;
import com.vaadin.terminal.StreamResource;
import com.vaadin.ui.AbstractSelect.ItemDescriptionGenerator;
import com.vaadin.ui.Button.ClickEvent;
import com.vaadin.ui.Button.ClickListener;
import com.vaadin.ui.Component;
import com.vaadin.ui.Label;
import com.vaadin.ui.Table;
import com.vaadin.ui.Window.Notification;

@SuppressWarnings("serial")

public class MDSAppView extends RefVaadinBasePanel implements ItemClickListener {
	
	static final Action			ACTION_US_EDIT_ALIAS						= new Action("Edit Unit Server Alias");
	static final Action			ACTION_US_LOAD_ALL							= new Action("Load all Control Units");
	static final Action			ACTION_US_UNLOAD_ALL						= new Action("Unload all Control Units");
	static final Action			ACTION_US_SHOW_ALL							= new Action("Edit Control Units");
	static final Action			ACTION_US_CREATE_CU							= new Action("New Control Unit");
	
	static final Action			ACTION_NODE_START						    = new Action("Node Start");
	static final Action			ACTION_NODE_STOP							= new Action("Node Stop");
	static final Action			ACTION_NODE_INIT							= new Action("Node Init");
	static final Action			ACTION_NODE_DEINIT							= new Action("Node DeInit");
	static final Action			ACTION_NODE_SHUTDOWN						= new Action("Node ShutDown");
	static final Action			ACTION_EDIT								= new Action("Edit");
	static final Action			ACTION_SAVE								= new Action("Save");

	
	static final Action			ACTION_EDIT_CU								= new Action("Edit..");
	
	static final Action			ACTION_CU_LOAD						   		= new Action("CU Load");
	static final Action			ACTION_CU_UNLOAD							= new Action("CU Unload");
	static final Action			ACTION_CU_COPY								= new Action("Copy");

	static final Action[]		ACTIONS_TABLE_EDIT						= new Action[] { ACTION_EDIT_CU };
	static final Action[]		ACTIONS_OF_REGISTERED_US				= new Action[] { ACTION_US_EDIT_ALIAS, ACTION_US_LOAD_ALL, ACTION_US_UNLOAD_ALL};
	static final Action[]		NODE_ACTIONS								= new Action[] { ACTION_NODE_START, ACTION_NODE_STOP, ACTION_NODE_INIT, ACTION_NODE_DEINIT/*,ACTION_NODE_SHUTDOWN*/ };

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

	// device action events
	
	public static final String	EVENT_NODE_SHUTDOWN							= "MDSAppView_EVENT_NODE_SHUTDOWN";


	//
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
	public static final String  EVENT_CU_REGISTERED 						= "MDSAppView_EVENT_CU_REGISTERED";

	public static final String	TAB1_DEV_STATUS								= "Status";

	public static final String	TAB1_DEVICE_INSTANCE						= "Device Instance";
	public static final String	TAB1_DEVICE_US								= "US";

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
	public static final String	TAB_UNIT_SERVER_REGISTERED					= "registered";
	public static final String	KEY_USERVER_CUTYPE_TAB						= "KEY_USERVER_CUTYPE_TAB";
	public static final String	TAB_UNIT_SERVER_CUTYPE_ALIAS_NAME			= "CU ID";
	public static final String	TAB_UNIT_SERVER_CUTYPE_TYPE_NAME			= "CU Type Name";
	public static final String	TAB_UNIT_SERVER_CUTYPE_INTERFACE_NAME		= "CU Interface";
	public static final String	TAB_UNIT_SERVER_CUTYPE_STATE				= "CU State";
	public static final String	TAB_UNIT_SERVER_CUTYPE_SM					= "CU SM";

	public static final String	KEY_DEVICE_TAB								= "DEVICE_TAB";
	public static final String	KEY_DATASET_TAB								= "KEY_DATASET_TAB";
	public static final String	KEY_DEVICE_START_AT_INIT_BUTTON				= "KEY_DEVICE_START_AT_INIT_BUTTON";
	public static final String	KEY_DATASET_ATTRIBUTE_TAB					= "KEY_DATASET_ATTRIBUTE_TAB";

	private MainDynView			mv											= new MainDynView();
	 
	private boolean				editingAttribute							= false;
	public static Refresher	 	refresher									= new Refresher();
	public static ChaosEventComponent	chaosEventComponent					= ChaosEventComponent.getInstance();
	private String				selectedUnit								= null;
	private int					nrepaint									= 0; 
	private static 	long		time_to_dead								= 60*1000;
	private static String 		stateRegex									= "\\w+\\:(.+)\\s+\\w+\\:(.+)";
	@Override
	public void initGui() {
		
		refresher.setRefreshInterval(5000); 
		addComponent(refresher);
		addComponent(mv);
		
		refresher.addListener(new Refresher.RefreshListener() {
			private static final long serialVersionUID = -8765221895426102605L;
			public void refresh(Refresher source) {
				if(nrepaint>0){
					notifyEventoToControllerWithData(MDSUIEvents.EVENT_REFRESH_STATE, source, null);
				}
			}
		});
			
		//addComponent(chaosEventComponent);
		
		mv.setWidth("100.0%");
		mv.setHeight("100.0%");
		setComponentKey(KEY_USERVER_TAB, mv.getTableUnitServer());
		setComponentKey(KEY_USERVER_CUTYPE_TAB, mv.getTableUnitServerCUType());

		setComponentKey(KEY_DEVICE_TAB, mv.getTableDevice());
//		setComponentKey(KEY_DEVICE_START_AT_INIT_BUTTON, mv.getButtonInitializedAtStartup());


		chaosEventComponent.addListener(new ChaosEventListener() {


			@Override
			public void event(VaadinEvent source) {
				if(source.getEventKind() == ChaosEventComponent.CHAOS_EVENT_CU_REGISTERED){
					notifyEventoToControllerWithData(MDSUIEvents.EVENT_CU_REGISTERED, this, source.getData());

				} else if(source.getEventKind() == ChaosEventComponent.CHAOS_EVENT_UI_REGISTERED){
					notifyEventoToControllerWithData(MDSUIEvents.EVENT_UI_REGISTERED, this, source.getData());

				} else if(source.getEventKind() == ChaosEventComponent.CHAOS_EVENT_ERROR){
					notifyEventoToControllerWithData(MDSUIEvents.EVENT_CHAOS_ERROR, this, source.getData());

				} else if(source.getEventKind() == ChaosEventComponent.CHAOS_EVENT_US_UPDATE){
					notifyEventoToControllerWithData(MDSUIEvents.CHAOS_EVENT_US_UPDATE, this, source.getData());

				}				

			}
		});
//////////// US
		mv.getTableUnitServer().addListener(this);
		mv.getTableUnitServer().setEditable(false);
		mv.getTableUnitServer().setSelectable(true);
		// mv.getTableUnitServer().setImmediate(true);
		// mv.getTableUnitServer().setReadThrough(true);
		mv.getTableUnitServer().addContainerProperty(TAB_UNIT_SERVER_NAME, String.class, null);
		mv.getTableUnitServer().addContainerProperty(TAB_UNIT_SERVER_ADDRESS, String.class, null);
		mv.getTableUnitServer().addContainerProperty(TAB_UNIT_SERVER_HB_TS, Date.class, null);
		mv.getTableUnitServer().addContainerProperty(TAB_UNIT_SERVER_REGISTERED, String.class, null);

		mv.getTableUnitServer().addStyleName("chaosmdslitetheme");
		mv.getTableUnitServer().setCellStyleGenerator(new Table.CellStyleGenerator() {
		    public String getStyle(Object itemId, Object propertyId) {
		        // Row style setting, not relevant in this example.
		        if (propertyId == null)
		            return "green"; // Will not actually be visible

		       // int row = ((Integer)itemId).intValue();
		       // int col = Integer.parseInt((String)propertyId);
		        Item it=mv.getTableUnitServer().getItem(itemId);
		        String prop = it.getItemProperty(TAB_UNIT_SERVER_REGISTERED).toString();
		        String timestamp = it.getItemProperty(TAB_UNIT_SERVER_HB_TS).toString();
		        Date dt =null;
		        SimpleDateFormat formatter = new SimpleDateFormat("EEE MMM dd HH:mm:ss zzz yyyy", Locale.ENGLISH);
		        try {
					dt = formatter.parse(timestamp);
				} catch (ParseException e) {
					e.printStackTrace();
					return "red";
				}
		        Date now = new Date();
		        int val=0;
		       
		        if(prop==null){
		        	return "red";
		        } else if((now.getTime() - dt.getTime())>time_to_dead ){
		        	return "black";
		        } else {
		        	return "green";
		        }
		           
		    }
		});
		mv.getTableUnitServer().addActionHandler(new Action.Handler() {
			public Action[] getActions(Object target, Object sender) {
				return new Action[]{ACTION_US_EDIT_ALIAS};
			}

			public void handleAction(Action action, Object sender, Object target) {
				setEditingAttribute(!isEditingAttribute());
				if (ACTION_US_EDIT_ALIAS == action) {
					notifyEventoToControllerWithData(EVENT_UNIT_SERVER_EDIT_ALIAS, mv.getTableUnitServer().getValue(), sender);
				}
			}

		});
		mv.getButtonNewCU().setEnabled(false);
		mv.getButtonNewCU().setImmediate(true);
		mv.getButtonNewUS().addListener(new ClickListener() {
			public void buttonClick(ClickEvent event) {
				notifyEventoToControllerWithData(EVENT_UNIT_SERVER_NEW, event.getSource(), null);
			}
		});

		mv.getButtonDelUS().addListener(new ClickListener() {
			public void buttonClick(ClickEvent event) {
				ConfirmDialog.show(getWindow(), "Do you want to delete US and all the CUs: " +event.getSource().toString() + " ?" ,
				        new ConfirmDialog.Listener() {
							ClickEvent my_ev;

				            public void onClose(ConfirmDialog dialog) {
				                if (dialog.isConfirmed()) {
				    				notifyEventoToControllerWithData(EVENT_UNIT_SERVER_DELETE, my_ev.getSource(), null);
				                }
				            }
				            private ConfirmDialog.Listener init(ClickEvent ev){my_ev = ev; return this;}
				        }.init(event));

			}
		});
		
	/////////////////
		
		
   /////////////// DEVICE
		mv.getTableDevice().addListener(this);
		mv.getTableDevice().setEditable(false);
		mv.getTableDevice().setSelectable(true);
		mv.getTableDevice().setImmediate(true);
		mv.getTableDevice().setReadThrough(true);
		mv.getTableDevice().setNullSelectionAllowed(false);
		mv.getTableDevice().addContainerProperty(TAB1_DEVICE_INSTANCE, String.class, null);
	/*
		mv.getTableDevice().addContainerProperty(TAB1_DEVICE_US, String.class, null);
		mv.getTableDevice().addContainerProperty(TAB1_DEV_STATUS, String.class, null);
*/
		mv.getTableDevice().addContainerProperty(TAB1_NET_ADDRESS, String.class, null);
		mv.getTableDevice().addContainerProperty(TAB1_LAST_HB, Date.class, null);
		mv.getTableDevice().addContainerProperty(TAB2_TIMESTAMP, Date.class, null);
		mv.getTableDevice().addContainerProperty(TAB2_ATTR_NUMBER, Integer.class, null);
		
		mv.getTableDevice().addActionHandler(new Action.Handler() {
			public Action[] getActions(Object target, Object sender) {
				return NODE_ACTIONS;
			}

			
			@Override
			public void handleAction(Action action, Object sender, Object target) {
				if (ACTION_NODE_START == action) {
					Device dev=(Device) mv.getTableDevice().getValue();
					notifyEventoToControllerWithData(MDSUIEvents.EVENT_NODE_START, sender, dev.getDeviceIdentification());
				}else if (ACTION_NODE_STOP == action) {
					notifyEventoToControllerWithData(MDSUIEvents.EVENT_NODE_STOP,  sender, mv.getTableDevice().getValue());
				}else if (ACTION_NODE_INIT == action) {
					notifyEventoToControllerWithData(MDSUIEvents.EVENT_NODE_INIT, sender, mv.getTableDevice().getValue());
				}else if (ACTION_NODE_DEINIT == action) {
					notifyEventoToControllerWithData(MDSUIEvents.EVENT_NODE_DEINIT, sender, mv.getTableDevice().getValue());
				}else if (ACTION_NODE_SHUTDOWN == action) {
					notifyEventoToControllerWithData(EVENT_NODE_SHUTDOWN, sender, mv.getTableDevice().getValue());
				
				}
			}
			
		});
		
		mv.getTableDevice().setCellStyleGenerator(new Table.CellStyleGenerator() {
		    public String getStyle(Object itemId, Object propertyId) {
		        // Row style setting, not relevant in this example.
		        if (propertyId == null)
		            return "green"; // Will not actually be visible

		       // int row = ((Integer)itemId).intValue();
		       // int col = Integer.parseInt((String)propertyId);
		        Item it=mv.getTableDevice().getItem(itemId);
		        String timestamp = it.getItemProperty(TAB1_LAST_HB).toString();
		        Date dt =null;
		        SimpleDateFormat formatter = new SimpleDateFormat("EEE MMM dd HH:mm:ss zzz yyyy", Locale.ENGLISH);
		        try {
					dt = formatter.parse(timestamp);
				} catch (ParseException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
		        Date now = new Date();
		       if((now.getTime() - dt.getTime())>time_to_dead ){
		        	return "red";
		        } else {
		        	return "green";
		        }
		           
		    }
		});

	///////////////////// CU  ///////////
		 
		
	
		mv.getTableUnitServerCUType().addListener(this);
		mv.getTableUnitServerCUType().setEditable(false);
		mv.getTableUnitServerCUType().setSelectable(true);
		mv.getTableUnitServerCUType().setImmediate(true);
		mv.getTableUnitServerCUType().setReadThrough(true);
		mv.getTableUnitServerCUType().addContainerProperty(TAB_UNIT_SERVER_CUTYPE_ALIAS_NAME, String.class, null);
		mv.getTableUnitServerCUType().addContainerProperty(TAB_UNIT_SERVER_CUTYPE_TYPE_NAME, String.class, null);
		mv.getTableUnitServerCUType().addContainerProperty(TAB_UNIT_SERVER_CUTYPE_INTERFACE_NAME, String.class, null);
		mv.getTableUnitServerCUType().addContainerProperty(TAB_UNIT_SERVER_CUTYPE_STATE, String.class, null);
		mv.getTableUnitServerCUType().addContainerProperty(TAB_UNIT_SERVER_CUTYPE_SM, String.class, null);

		mv.getTableUnitServerCUType().addActionHandler(new Action.Handler() {
			public Action[] getActions(Object target, Object sender) {
				return new Action[] { ACTION_EDIT_CU,ACTION_CU_COPY,ACTION_CU_LOAD,ACTION_CU_UNLOAD };
			}

			public void handleAction(Action action, Object sender, Object target) {
				if (ACTION_EDIT_CU == action) {
					if (mv.getTableUnitServerCUType().getValue() == null) {
						getWindow().showNotification("Edit association error", "Not A legal Value", Notification.TYPE_ERROR_MESSAGE);
						return;
					} else {
						notifyEventoToControllerWithData(MDSUIEvents.EVENT_EDIT_ASSOCIATION, sender, mv.getTableUnitServerCUType().getValue());
					} 
				} else if (ACTION_CU_LOAD == action) {
					Set<UnitServerCuInstance> scu= new HashSet<UnitServerCuInstance>();
					scu.add((UnitServerCuInstance) mv.getTableUnitServerCUType().getValue());
					notifyEventoToControllerWithData(MDSUIEvents.EVENT_LOAD_INSTANCE, sender, scu);
					
				} else if (ACTION_CU_UNLOAD == action) {
					Set<UnitServerCuInstance> scu= new HashSet<UnitServerCuInstance>();
					scu.add((UnitServerCuInstance) mv.getTableUnitServerCUType().getValue());
					notifyEventoToControllerWithData(MDSUIEvents.EVENT_UNLOAD_INSTANCE, sender, scu);

				} else if (ACTION_CU_COPY == action) {
					Set<UnitServerCuInstance> scu= new HashSet<UnitServerCuInstance>();
					scu.add((UnitServerCuInstance) mv.getTableUnitServerCUType().getValue());
					notifyEventoToControllerWithData(MDSUIEvents.EVENT_COPY_CU, sender, mv.getTableUnitServerCUType().getValue());
				}
			}

		});
		
		mv.getButtonNewCU().addListener(new ClickListener() {
			public void buttonClick(ClickEvent event) {
				if(mv.getTableUnitServer().getValue()!=null){
					Table tb = mv.getTableUnitServer();
					Item it = tb.getItem(tb.getValue());
					String USname = it.getItemProperty(TAB_UNIT_SERVER_NAME).toString();
					notifyEventoToControllerWithData(EVENT_UNIT_SERVER_CREATE_US_CU_ASSOCIATION, event.getSource(), USname);
			}
			}
		});

		mv.getButtonDelCU().addListener(new ClickListener() {
			public void buttonClick(ClickEvent event) {
				if(mv.getTableUnitServerCUType().getValue()!=null){
					ConfirmDialog.show(getWindow(), "Do you want to delete CU: " +mv.getTableUnitServerCUType().getValue().toString() + "?" ,
					        new ConfirmDialog.Listener() {
								ClickEvent my_ev;

					            public void onClose(ConfirmDialog dialog) {
					                if (dialog.isConfirmed()) {
										notifyEventoToControllerWithData(MDSUIEvents.EVENT_REMOVE_ASSOCIATION, my_ev.getSource(), mv.getTableUnitServerCUType().getValue());
					                }
					            }
					            private ConfirmDialog.Listener init(ClickEvent ev){my_ev = ev; return this;}
					        }.init(event));
			
				} else {}		
				
			}	
		});
///////////////////
		
		mv.getButtonPreference().addListener(new ClickListener() {
			public void buttonClick(ClickEvent event) {
				notifyEventoToControllerWithData(EVENT_SHOW_PREFERENCE, event.getSource(), null);
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
				return new Action[] { ACTION_EDIT,ACTION_SAVE };
			}

			public void handleAction(Action action, Object sender, Object target) {
				setEditingAttribute(!isEditingAttribute());
				if (ACTION_EDIT == action) {
					mv.getTableDatasetAttribute().setEditable(!mv.getTableDatasetAttribute().isEditable());
					mv.getTableDatasetAttribute().refreshRowCache();
				} else if (ACTION_SAVE== action) {
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

	private String[] getState(String state){
		String[] ret= new String[2];
		ret[0] = null;
		ret[1] = null;
		Pattern p= Pattern.compile(stateRegex);
		Matcher m= p.matcher(state);
		if(state!=null){
			ret[0] = state;
			ret[1] = state;
		}
		while(m.find()){

			ret[0]=m.group(1);
			ret[1]=m.group(2);
		}
		
		return ret;
	}


	/*
	 * (non-Javadoc)
	 * 
	 * @see org.ref.server.webapp.RefVaadinBasePanel#update(java.util.Observable, java.lang.Object)
	 */
	public void update(Observable source, Object sourceData) {
	

		Date now = new Date();
		if (sourceData instanceof ViewNotifyEvent) {
			ViewNotifyEvent viewEvent = (ViewNotifyEvent) sourceData;
			nrepaint++;
			if (viewEvent.getEventKind().equals(MDSAppView.EVENT_ATTRIBUTE_EDITING)) {
				setEditingAttribute(true);
				mv.getTableDatasetAttribute().setEditable(true);
				mv.getTableDatasetAttribute().refreshRowCache();
			} else if(viewEvent.getEventKind().equals(MDSUIEvents.EVENT_UPDATE_US)){
				// load all dataset
				List<UnitServer> allUnitServer = (List<UnitServer>)viewEvent.getEventData();
				Table t = (Table) mv.getTableUnitServer();
				t.removeAllItems();
				for (UnitServer us : allUnitServer) {
					Item woItem = t.addItem(us.getAlias());
					woItem.getItemProperty(MDSAppView.TAB_UNIT_SERVER_NAME).setValue(us.getAlias());
					woItem.getItemProperty(MDSAppView.TAB_UNIT_SERVER_ADDRESS).setValue(us.getIp_port());
					woItem.getItemProperty(MDSAppView.TAB_UNIT_SERVER_HB_TS).setValue(us.getUnitServerHB().getDate());
					if((now.getTime() - us.getUnitServerHB().getDate().getTime())>time_to_dead){
						
						woItem.getItemProperty(MDSAppView.TAB_UNIT_SERVER_REGISTERED).setValue("---");
					} else {
						
						woItem.getItemProperty(MDSAppView.TAB_UNIT_SERVER_REGISTERED).setValue(us.getState());
					}
					
				}
			} else if(viewEvent.getEventKind().equals(MDSUIEvents.EVENT_REFRESH_US)){
				// load all dataset
				if(viewEvent.getEventData() == null){
					mv.getTableUnitServer().requestRepaint();
					return;
				}
				UnitServer us = (UnitServer)viewEvent.getEventData();
				Table t = (Table) mv.getTableUnitServer();
				Item woItem = t.getItem(us.getAlias());
			
				if(woItem!=null){
					woItem.getItemProperty(MDSAppView.TAB_UNIT_SERVER_NAME).setValue(us.getAlias());
					woItem.getItemProperty(MDSAppView.TAB_UNIT_SERVER_ADDRESS).setValue(us.getIp_port());
					woItem.getItemProperty(MDSAppView.TAB_UNIT_SERVER_HB_TS).setValue(us.getUnitServerHB().getDate());
					
					if((now.getTime() - us.getUnitServerHB().getDate().getTime())>time_to_dead){
						
						woItem.getItemProperty(MDSAppView.TAB_UNIT_SERVER_REGISTERED).setValue("---");
					} else {
						woItem.getItemProperty(MDSAppView.TAB_UNIT_SERVER_REGISTERED).setValue(us.getState());
					}
				}
			} else if (viewEvent.getEventKind().equals(MDSUIEvents.EVENT_UPDATE_LIST)) {
				mv.getTableUnitServerCUType().removeAllItems();
				
				Vector<UnitServerCuInstance> currentInstancesForUnitServer = (Vector<UnitServerCuInstance>) viewEvent.getEventData();
				if ((currentInstancesForUnitServer == null) || (currentInstancesForUnitServer.size()<=0))
					return;
				
				selectedUnit = currentInstancesForUnitServer.get(0).getUnitServerAlias();
				Table us = (Table) mv.getTableUnitServer();
				Item it = us.getItem(us.getValue());
				us.removeStyleName("red");
				if(it!=null){
					String stat = it.getItemProperty(MDSAppView.TAB_UNIT_SERVER_REGISTERED).toString();
					if(stat!=null && stat.equals("---")){
						us.setStyleName("red");
					} 
				}
			
				
				for (UnitServerCuInstance unitServerCuInstance : currentInstancesForUnitServer) {
					Item woItem = mv.getTableUnitServerCUType().addItem(unitServerCuInstance);
					woItem.getItemProperty(TAB_UNIT_SERVER_CUTYPE_ALIAS_NAME).setValue(unitServerCuInstance.getCuId());
					woItem.getItemProperty(TAB_UNIT_SERVER_CUTYPE_TYPE_NAME).setValue(unitServerCuInstance.getCuType());
					woItem.getItemProperty(TAB_UNIT_SERVER_CUTYPE_INTERFACE_NAME).setValue(unitServerCuInstance.getInterface());
					String[] mystate=getState(unitServerCuInstance.getState());
					if(mystate[0]!=null){
						woItem.getItemProperty(TAB_UNIT_SERVER_CUTYPE_STATE).setValue(mystate[0]);
					} else {
						woItem.getItemProperty(TAB_UNIT_SERVER_CUTYPE_STATE).setValue("---");
					}
					if(mystate[1]!=null){
						woItem.getItemProperty(TAB_UNIT_SERVER_CUTYPE_SM).setValue(mystate[1]);
					} else {
						woItem.getItemProperty(TAB_UNIT_SERVER_CUTYPE_SM).setValue("---");

					}
				}
				
			} else if (viewEvent.getEventKind().equals(MDSUIEvents.EVENT_REFRESH_LIST)) {
				UnitServer us = (UnitServer) viewEvent.getEventData();
				if(us==null)
					return;
				for(UnitServerCuInstance unitServerCuInstance:us.getCuInstances()){

					Item woItem =mv.getTableUnitServerCUType().getItem(unitServerCuInstance);
					if(woItem==null)
						return;
					
					woItem.getItemProperty(TAB_UNIT_SERVER_CUTYPE_ALIAS_NAME).setValue(unitServerCuInstance.getCuId());
					woItem.getItemProperty(TAB_UNIT_SERVER_CUTYPE_TYPE_NAME).setValue(unitServerCuInstance.getCuType());
					if(unitServerCuInstance.getInterface()!=null)
						woItem.getItemProperty(TAB_UNIT_SERVER_CUTYPE_INTERFACE_NAME).setValue(unitServerCuInstance.getInterface());
					String state = unitServerCuInstance.getState();
						
					String[] mystate=getState(state);
					if((now.getTime() - us.getUnitServerHB().getDate().getTime() )  > time_to_dead){
						woItem.getItemProperty(TAB_UNIT_SERVER_CUTYPE_STATE).setValue("---");
						woItem.getItemProperty(TAB_UNIT_SERVER_CUTYPE_SM).setValue("---");

					} else {
						if(mystate[0]!=null){
							woItem.getItemProperty(TAB_UNIT_SERVER_CUTYPE_STATE).setValue(mystate[0]);	
						} else {
							woItem.getItemProperty(TAB_UNIT_SERVER_CUTYPE_STATE).setValue("---");
						}
						if(mystate[1]!=null){
							woItem.getItemProperty(TAB_UNIT_SERVER_CUTYPE_SM).setValue(mystate[1]);

						} else {
							woItem.getItemProperty(TAB_UNIT_SERVER_CUTYPE_SM).setValue("---");

						}
						//woItem.getItemProperty(TAB_UNIT_SERVER_CUTYPE_STATE).setValue(unitServerCuInstance.getState());
					}
				}
			} else if (viewEvent.getEventKind().equals(MDSUIEvents.EVENT_UPDATE_DEVICE_LIST)) {
				List<Device> allDevices = (List<Device>) viewEvent.getEventData();
				int i =0;
				Table t = mv.getTableDevice();
				t.removeAllItems();
				for(Device device:allDevices){
					Item woItem =  t.addItem(device);
					woItem.getItemProperty(MDSAppView.TAB1_DEVICE_INSTANCE).setValue(device.getDeviceIdentification());
/*					if(device.getParent()!=null){
						woItem.getItemProperty(MDSAppView.TAB1_DEVICE_US).setValue(device.getParent().getUnitServerAlias());
						woItem.getItemProperty(MDSAppView.TAB1_DEV_STATUS).setValue(device.getParent().getState());
					}
					*/
					woItem.getItemProperty(MDSAppView.TAB1_NET_ADDRESS).setValue(device.getNetAddress());
					woItem.getItemProperty(MDSAppView.TAB1_LAST_HB).setValue(device.getLastHeartBeatTimestamp() != null ? device.getLastHeartBeatTimestamp().getDate() : null);
					woItem.getItemProperty(MDSAppView.TAB2_TIMESTAMP).setValue(device.getLastDatasetForDevice().getDate());
					woItem.getItemProperty(MDSAppView.TAB2_ATTR_NUMBER).setValue(device.getAttributes());
				}
			} else if (viewEvent.getEventKind().equals(MDSUIEvents.EVENT_REFRESH_DEVICE_LIST)) {
				List<Device> allDevices = (List<Device>) viewEvent.getEventData();
				int i =0;
				Table t = mv.getTableDevice();
				for(Device device:allDevices){
					Item woItem =  t.getItem(device);
					if(woItem == null)
						return;
					woItem.getItemProperty(MDSAppView.TAB1_DEVICE_INSTANCE).setValue(device.getDeviceIdentification());
	/*				if(device.getParent()!=null){
						woItem.getItemProperty(MDSAppView.TAB1_DEVICE_US).setValue(device.getParent().getUnitServerAlias());
						woItem.getItemProperty(MDSAppView.TAB1_DEV_STATUS).setValue(device.getParent().getState());
					}
					*/
					woItem.getItemProperty(MDSAppView.TAB1_NET_ADDRESS).setValue(device.getNetAddress());
					woItem.getItemProperty(MDSAppView.TAB1_LAST_HB).setValue(device.getLastHeartBeatTimestamp() != null ? device.getLastHeartBeatTimestamp().getDate() : null);
					woItem.getItemProperty(MDSAppView.TAB2_TIMESTAMP).setValue(device.getLastDatasetForDevice().getDate());
					woItem.getItemProperty(MDSAppView.TAB2_ATTR_NUMBER).setValue(device.getAttributes());
				}
			} else if (viewEvent.getEventKind().equals(MDSUIEvents.CTRL_CONFIG_GENERATED)) {
			
				String filename = (String)viewEvent.getEventData();
				if(filename==null){
					return;
				}
				
				
				FileResource ssource = new FileResource(filename);
				
	       
			   Date dd = new Date();
			   StreamResource resource = new StreamResource(ssource, "MDSConfig_"+(dd.getTime()/1000) +" .txt",getApplication());
			   resource.setMIMEType("application/octet-stream");

			   getWindow().open(resource,"_blank");
			  // ssource.removeResource();
				
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
		/*if (event.getSource().equals(mv.getButtonUSCULoadAll())) {
			notifyEventoToControllerWithData(EVENT_UNIT_SERVER_LOAD_ALL_ASSOCIATION, event.getSource(), event.getItemId());
		} else if (event.getSource().equals(mv.getButtonUSCUUnloadAll())) {
			notifyEventoToControllerWithData(EVENT_UNIT_SERVER_UNLOAD_ALL_ASSOCIATION, event.getSource(), event.getItemId());
		} else if (event.getSource().equals(mv.getButtonUSCUShowAll())) {
			notifyEventoToControllerWithData(EVENT_UNIT_SERVER_SHOW_ALL_ASSOCIATION, event.getSource(), event.getItemId());
		} else*/ if (event.getSource().equals(mv.getTableUnitServer())) {
			if(mv.getTableUnitServer().getValue()==null){
				mv.getButtonNewCU().setEnabled(true); // the state is in late
			
			} else {
				mv.getButtonNewCU().setEnabled(false);	
			}
			Item it = event.getItem();
			String registered = it.getItemProperty(TAB_UNIT_SERVER_REGISTERED).toString();
			String prop = it.getItemProperty(TAB_UNIT_SERVER_REGISTERED).toString();
		    String timestamp = it.getItemProperty(TAB_UNIT_SERVER_HB_TS).toString();
		    Date dt =null;
	        SimpleDateFormat formatter = new SimpleDateFormat("EEE MMM dd HH:mm:ss zzz yyyy", Locale.ENGLISH);
	        try {
				dt = formatter.parse(timestamp);
			} catch (ParseException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
	       // Date now = new Date();
			mv.getTableUnitServer().removeAllActionHandlers();
			mv.getTableUnitServerCUType().removeAllActionHandlers();
			int regvalue= 0;
			try{
				regvalue = Integer.valueOf(registered);
			} catch(Throwable e){
				
			}
			if(registered!=null && (!registered.equals("---"))){
				mv.getTableUnitServer().addActionHandler(new Action.Handler() {
					public Action[] getActions(Object target, Object sender) {
						return ACTIONS_OF_REGISTERED_US;
					}

					public void handleAction(Action action, Object sender, Object target) {
						setEditingAttribute(!isEditingAttribute());
						if (ACTION_US_EDIT_ALIAS == action) {
							notifyEventoToControllerWithData(EVENT_UNIT_SERVER_EDIT_ALIAS, mv.getTableUnitServer().getValue(), sender);
						}else if (ACTION_US_LOAD_ALL == action) {
							notifyEventoToControllerWithData(EVENT_UNIT_SERVER_LOAD_ALL_ASSOCIATION, null, null);
						}else if (ACTION_US_UNLOAD_ALL == action) {
							notifyEventoToControllerWithData(EVENT_UNIT_SERVER_UNLOAD_ALL_ASSOCIATION, null, sender);
						}/*else if (ACTION_US_SHOW_ALL == action) {
							notifyEventoToControllerWithData(EVENT_UNIT_SERVER_SHOW_ALL_ASSOCIATION, null, sender);
						}else if (ACTION_US_CREATE_CU == action) {
							if(mv.getTableUnitServerCUType().getValue()!=null){
								notifyEventoToControllerWithData(EVENT_UNIT_SERVER_CREATE_US_CU_ASSOCIATION, null, sender);
							} else {
								getWindow().showNotification("New CU", "<b>A Type CU needs to be selected</b>", Window.Notification.TYPE_ERROR_MESSAGE);

							}
						}*/
					}

				});
				mv.getTableUnitServerCUType().addActionHandler(new Action.Handler() {
					public Action[] getActions(Object target, Object sender) {
						return new Action[] { ACTION_EDIT_CU,ACTION_CU_COPY,ACTION_CU_LOAD,ACTION_CU_UNLOAD };
					}

					public void handleAction(Action action, Object sender, Object target) {
						if (ACTION_EDIT_CU == action) {
							if (mv.getTableUnitServerCUType().getValue() == null) {
								getWindow().showNotification("Edit association error", "Not A legal Value", Notification.TYPE_ERROR_MESSAGE);
								return;
							} else {
								notifyEventoToControllerWithData(MDSUIEvents.EVENT_EDIT_ASSOCIATION, sender, mv.getTableUnitServerCUType().getValue());
							} 
						} else if (ACTION_CU_LOAD == action) {
							Set<UnitServerCuInstance> scu= new HashSet<UnitServerCuInstance>();
							scu.add((UnitServerCuInstance) mv.getTableUnitServerCUType().getValue());
							notifyEventoToControllerWithData(MDSUIEvents.EVENT_LOAD_INSTANCE, sender, scu);
							
						} else if (ACTION_CU_UNLOAD == action) {
							Set<UnitServerCuInstance> scu= new HashSet<UnitServerCuInstance>();
							scu.add((UnitServerCuInstance) mv.getTableUnitServerCUType().getValue());
							notifyEventoToControllerWithData(MDSUIEvents.EVENT_UNLOAD_INSTANCE, sender, scu);

						} else if (ACTION_CU_COPY == action) {
							Set<UnitServerCuInstance> scu= new HashSet<UnitServerCuInstance>();
							scu.add((UnitServerCuInstance) mv.getTableUnitServerCUType().getValue());
							notifyEventoToControllerWithData(MDSUIEvents.EVENT_COPY_CU, sender, mv.getTableUnitServerCUType().getValue());
						}
					}

				});
			} else {
				//notifyEventoToControllerWithData(MDSUIEvents.EVENT_REMOVE_CU_DEVICES,this,mv.getTableUnitServer().getValue());
				mv.getTableUnitServer().addActionHandler(new Action.Handler() {
					public Action[] getActions(Object target, Object sender) {
						return new Action[]{ACTION_US_EDIT_ALIAS};
					}

					public void handleAction(Action action, Object sender, Object target) {
						setEditingAttribute(!isEditingAttribute());
						if (ACTION_US_EDIT_ALIAS == action) {
							notifyEventoToControllerWithData(EVENT_UNIT_SERVER_EDIT_ALIAS, mv.getTableUnitServer().getValue(), sender);
						}
					}

					

				});
				mv.getTableUnitServerCUType().addActionHandler(new Action.Handler() {
					public Action[] getActions(Object target, Object sender) {
						return new Action[] { ACTION_EDIT_CU,ACTION_CU_COPY};
					}

					public void handleAction(Action action, Object sender, Object target) {
						if (ACTION_EDIT_CU == action) {
							if (mv.getTableUnitServerCUType().getValue() == null) {
								getWindow().showNotification("Edit association error", "Not A legal Value", Notification.TYPE_ERROR_MESSAGE);
								return;
							} else {
								notifyEventoToControllerWithData(MDSUIEvents.EVENT_EDIT_ASSOCIATION, sender, mv.getTableUnitServerCUType().getValue());
							} 
						} else if (ACTION_CU_COPY == action) {
							Set<UnitServerCuInstance> scu= new HashSet<UnitServerCuInstance>();
							scu.add((UnitServerCuInstance) mv.getTableUnitServerCUType().getValue());
							notifyEventoToControllerWithData(MDSUIEvents.EVENT_COPY_CU, sender, mv.getTableUnitServerCUType().getValue());
						} 
					}

				});
			}
			
			
			notifyEventoToControllerWithData(EVENT_UNIT_SERVER_SELECTED, event.getSource(), event.getItemId());
			
		} else if (event.getSource().equals(mv.getTableUnitServerCUType())) {
			mv.getButtonDelCU().setEnabled(true);
			notifyEventoToControllerWithData(EVENT_UNIT_SERVER_CU_TYPE_SELECTED, event.getSource(), event.getItemId());
		} else if (event.getSource().equals(mv.getTableDevice())) {
			Table t = mv.getTableDevice();
			 Item it  = t.getItem(event.getItemId());
			  String timestamp = it.getItemProperty(TAB1_LAST_HB).toString();
		        Date dt =null;
		        SimpleDateFormat formatter = new SimpleDateFormat("EEE MMM dd HH:mm:ss zzz yyyy", Locale.ENGLISH);
		        try {
					dt = formatter.parse(timestamp);
				} catch (ParseException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
		        Date now = new Date();
		       if((now.getTime() - dt.getTime())>time_to_dead){
		        	t.removeAllActionHandlers();
		        	
		        } else {
		        	t.removeAllActionHandlers();
		        	t.addActionHandler(new Action.Handler() {
		    			public Action[] getActions(Object target, Object sender) {
		    				return NODE_ACTIONS;
		    			}
		    			@Override
		    			public void handleAction(Action action, Object sender, Object target) {
		    				if (ACTION_NODE_START == action) {
		    					notifyEventoToControllerWithData(MDSUIEvents.EVENT_NODE_START, sender, mv.getTableDevice().getValue());
		    				}else if (ACTION_NODE_STOP == action) {
		    					notifyEventoToControllerWithData(MDSUIEvents.EVENT_NODE_STOP,  sender, mv.getTableDevice().getValue());
		    				}else if (ACTION_NODE_INIT == action) {
		    					notifyEventoToControllerWithData(MDSUIEvents.EVENT_NODE_INIT, sender, mv.getTableDevice().getValue());
		    				}else if (ACTION_NODE_DEINIT == action) {
		    					notifyEventoToControllerWithData(MDSUIEvents.EVENT_NODE_DEINIT, sender, mv.getTableDevice().getValue());
		    				}else if (ACTION_NODE_SHUTDOWN == action) {
		    					notifyEventoToControllerWithData(EVENT_NODE_SHUTDOWN, sender, mv.getTableDevice().getValue());
		    				
		    				}
		    			}
		    			
		    		});
		        }
		       
			notifyEventoToControllerWithData(EVENT_DEVICE_SELECTED, event.getSource(), event.getItemId());
			
		} /*else if (event.getSource().equals(mv.getTableDataset())) {
			notifyEventoToControllerWithData(EVENT_DATASET_SELECTED, event.getSource(), event.getItemId());
		}*/ else if (event.getSource().equals(mv.getTableDatasetAttribute())) {
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

	
	
