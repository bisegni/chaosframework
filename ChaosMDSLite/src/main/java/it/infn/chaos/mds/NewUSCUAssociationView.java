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

import com.vaadin.data.Item;
import com.vaadin.event.Action;
import com.vaadin.event.FieldEvents.TextChangeEvent;
import com.vaadin.event.FieldEvents.TextChangeListener;
import com.vaadin.event.ItemClickEvent;
import com.vaadin.event.ItemClickEvent.ItemClickListener;
import com.vaadin.ui.Button.ClickEvent;
import com.vaadin.ui.Button.ClickListener;
import com.vaadin.ui.Window.Notification;
import com.vaadin.ui.Table;
import com.vaadin.ui.Window;

import org.vaadin.dialogs.ConfirmDialog;

/**
 * @author bisegni
 * 
 */
public class NewUSCUAssociationView extends RefVaadinBasePanel implements com.vaadin.data.Container.PropertySetChangeListener, ItemClickListener {
	
	private static final String				ATTRIBUTE_DEFAULT_VALUE			= "Default Value";
	private static final String				ATTRIBUTE_MIN_VALUE				= "Min Value";
	private static final String				ATTRIBUTE_MAX_VALUE				= "Max Value";
	private static final String				ATTRIBUTE_NAME					= "Attribute name";
	
	
	private static final long			serialVersionUID				= -841710385419733478L;

	
	
	static final Action					ACTION_EDIT						= new Action("Edit");
	static final Action					ACTION_SAVE						= new Action("Save");
	static final Action[]				ACTIONS_TO_EDIT					= new Action[] { ACTION_EDIT };
	static final Action[]				ACTIONS_IN_EDIT					= new Action[] { ACTION_SAVE };
	public static final String			SET_SC_CUTYPE_VALUE				= "SET_SC_CUTYPE_VALUE";
	public static final Object			SET_ASSOCIATION_TO_EDIT			= "SET_ASSOCIATION_TO_EDIT";
	public static final String			EVENT_CANCEL_USCU_ASSOC_VIEW	= "EVENT_CANCEL_USCU_ASSOC_VIEW";
	public static final String			EVENT_SAVE_USCU_ASSOC_VIEW		= "EVENT_SAVE_USCU_ASSOC_VIEW";
	public static final String			EVENT_ADD_DRIVER_SPEC			= "EVENT_ADD_DRIVER_SPEC";
	public static final String			EVENT_REMOVE_DRIVER_SPEC		= "EVENT_REMOVE_DRIVER_SPEC";
	public static final String			EVENT_LOAD_INSTANCE_ATTRIBUTE	= "USCUAssociationListView_EVENT_LOAD_INSTANCE_ATTRIBUTE";
	public static final String			EVENT_SAVE_ATTRIBUTE_CONFIG		= "USCUAssociationListView_EVENT_SAVE_ATTRIBUTE_CONFIG";


	private static final Object			TABLE_COLUMN_DRIVER_ALIAS		= "Name";
	private static final Object			TABLE_COLUMN_DRIVER_VERSION		= "Version";
	private static final Object			TABLE_COLUMN_DRIVER_INIT		= "Init Param";
	private NewUSCUAssociationVaadin	impl							= new NewUSCUAssociationVaadin();
	private boolean						editingServer					= false;
	private Object						driverSpecSelected				= null;
	private int							index							= 0;
	private UnitServerCuInstance		associationToEdit				= null;
	protected int						idx;
	protected int 						modified=0;
	/*
	 * (non-Javadoc)
	 * 
	 * @see org.ref.server.webapp.RefVaadinBasePanel#initGui()
	 */
	@SuppressWarnings("serial")
	@Override
	public void initGui() {
		addComponent(impl);
		// impl.getTableDriverSpecifications().addStyleName("components-inside");
		impl.getTableDriverSpecifications().addListener((ItemClickListener) this);
		// impl.getTableDriverSpecifications().setEditable(true);
		impl.getTableDriverSpecifications().setSelectable(true);
		impl.getTableDriverSpecifications().setImmediate(true);
		impl.getTableDriverSpecifications().setReadThrough(true);
		impl.getTableDriverSpecifications().setNullSelectionAllowed(true);
		impl.getTableDriverSpecifications().addContainerProperty(NewUSCUAssociationView.TABLE_COLUMN_DRIVER_ALIAS, String.class, null);
		impl.getTableDriverSpecifications().addContainerProperty(NewUSCUAssociationView.TABLE_COLUMN_DRIVER_VERSION, String.class, null);
		impl.getTableDriverSpecifications().addContainerProperty(NewUSCUAssociationView.TABLE_COLUMN_DRIVER_INIT, String.class, null);

		impl.getTableDriverSpecifications().addActionHandler(new Action.Handler() {
			public Action[] getActions(Object target, Object sender) {
				return impl.getTableDriverSpecifications().isEditable() ? ACTIONS_IN_EDIT : ACTIONS_TO_EDIT;
			}

			public void handleAction(Action action, Object sender, Object target) {
				impl.getTableDriverSpecifications().setEditable(!impl.getTableDriverSpecifications().isEditable());
				if (ACTION_EDIT == action) {
					impl.getTableDriverSpecifications().refreshRowCache();
				} else if (ACTION_SAVE == action) {
					impl.getTableDriverSpecifications().refreshRowCache();
					modified ++;

				}
			}

		});
		impl.getButtonAddDriverSpec().addListener(new ClickListener() {
			public void buttonClick(ClickEvent event) {
				// DriverSpec usci = new DriverSpec();
				// initParamterTextField.set
				Item woItem = impl.getTableDriverSpecifications().addItem(new Integer(index++));
				woItem.getItemProperty(NewUSCUAssociationView.TABLE_COLUMN_DRIVER_ALIAS).setValue("Name");
				woItem.getItemProperty(NewUSCUAssociationView.TABLE_COLUMN_DRIVER_VERSION).setValue("Version");
				woItem.getItemProperty(NewUSCUAssociationView.TABLE_COLUMN_DRIVER_INIT).setValue("Init Parameter");
			}
		});

		impl.getButtonRemoveDriverSpec().addListener(new ClickListener() {
			public void buttonClick(ClickEvent event) {
				impl.getTableDriverSpecifications().removeItem(driverSpecSelected);
			}
		});

		impl.getButtonCancel().addListener(new ClickListener() {
			public void buttonClick(ClickEvent event) {
				if(modified>0){
					ConfirmDialog.show(getWindow(), "You have modifications, Do you want to loose them?",
					        new ConfirmDialog.Listener() {
								ClickEvent my_ev;

					            public void onClose(ConfirmDialog dialog) {
					                if (dialog.isConfirmed()) {
					                	notifyEventoToControllerWithData(NewUSCUAssociationView.EVENT_CANCEL_USCU_ASSOC_VIEW, my_ev.getSource(), null);
					                }
					            }
					            private ConfirmDialog.Listener init(ClickEvent ev){my_ev = ev; return this;}
					        }.init(event));
					
					
				} else {
                	notifyEventoToControllerWithData(NewUSCUAssociationView.EVENT_CANCEL_USCU_ASSOC_VIEW, event.getSource(), null);

				}
				
			}
		});
		impl.getCheckBoxAutoLoad().addListener(new ClickListener(){
			public void buttonClick(ClickEvent event){
				modified++;
			}
		});
		
		impl.getCuIDTextField().addListener(new TextChangeListener(){

			@Override
			public void textChange(TextChangeEvent event) {
				modified++;
			}
			
		});
		



			

		impl.getButtonSave().addListener(new ClickListener() {
			public void buttonClick(ClickEvent event) {
				UnitServerCuInstance usci = associationToEdit == null ? new UnitServerCuInstance():associationToEdit;
				try {
					usci.setUnitServerAlias(impl.getUsSelected().getValue().toString());
					usci.setCuId(impl.getCuIDTextField().getValue().toString());
					usci.setCuType(impl.getCuTypeSelection().getValue().toString());
					usci.setCuParam(impl.getCuParamTextArea().getValue().toString());
					usci.setAutoLoad((Boolean) impl.getCheckBoxAutoLoad().getValue());
					
					usci.removeAllDriverSpec();
					Collection<Integer> indexList = (Collection<Integer>) impl.getTableDriverSpecifications().getItemIds();
					for (Integer idx : indexList) {
						DriverSpec driverSpec = new DriverSpec();
						driverSpec.setDriverName(impl.getTableDriverSpecifications().getItem(idx).getItemProperty(NewUSCUAssociationView.TABLE_COLUMN_DRIVER_ALIAS).toString());
						driverSpec.setDriverVersion(impl.getTableDriverSpecifications().getItem(idx).getItemProperty(NewUSCUAssociationView.TABLE_COLUMN_DRIVER_VERSION).toString());
						driverSpec.setDriverInit(impl.getTableDriverSpecifications().getItem(idx).getItemProperty(NewUSCUAssociationView.TABLE_COLUMN_DRIVER_INIT).toString());
						usci.addDrvierSpec(driverSpec);
					}
					
					usci.setDrvSpec(usci.getDriverDescriptionAsBson().toString());
					usci.checkIntegrityValues();
				} catch (Throwable e) {
					getWindow().showNotification( "Save Association", "Some value are not good");

				}
				
				//// save attributes
				UnitServerCuInstance selectedInstance = usci;
				

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
						getWindow().showNotification( "Attribute Name", "Not initialized", Notification.TYPE_ERROR_MESSAGE);
						return;
					}
					
					da.setName(sname);
					if(!smax.isEmpty()){
						try {
							max = Double.parseDouble(smax);
						} catch(Throwable e) {
							
							getWindow().showNotification( "Save Association", "<b>Attribute \""+sname+"\" MAX \""+smax+"\" bad format</b>", Notification.TYPE_ERROR_MESSAGE);
							return;
						}
					}
					if(!smin.isEmpty()){
						try {
							min = Double.parseDouble(smin);
						} catch(Throwable e) {
							getWindow().showNotification("Save Attribute Configuration Error", "<b>Attribute \""+sname+"\" MIN \"" +smin+"\" bad format</b>", Notification.TYPE_ERROR_MESSAGE);
							return;
						}
					}
					
					if(!sdef.isEmpty()){
						try {
							def = Double.parseDouble(sdef);
						} catch(Throwable e) {
							getWindow().showNotification("Save Attribute Configuration Error", "<b>Attribute \""+sname+"\" DEFAULT \""+sdef+"\" bad format</b>", Notification.TYPE_ERROR_MESSAGE);
							return;
						}
						
						if(min!=null && def<min){
							getWindow().showNotification("Save Attribute Configuration Error", "<b>Attribute \""+sname+"\" DEFAULT "+sdef+" cannot be lower than MIN "+smin+"</b>", Notification.TYPE_ERROR_MESSAGE);
							return;
							
						}
						if(max!=null && def>max){
							getWindow().showNotification("Save Attribute Configuration Error", "<b>Attribute \""+sname+"\" DEFAULT "+sdef+" cannot be greater than MAX "+smax+"</b>", Notification.TYPE_ERROR_MESSAGE);
							return;
							
						}
					}
					
					da.setDefaultValueNoCheck(sdef);
					
					da.setRangeMaxNoCheck(smax);
					da.setRangeMinNoCheck(smin);
					toSave.addElement(da);
					//notifyEventoToControllerWithData(NewUSCUAssociationView.EVENT_SAVE_USCU_ASSOC_VIEW, event.getSource(), usci);
				}
				selectedInstance.setAttributeConfigutaion(toSave);
				getWindow().showNotification("Attribute Configuration", "<b>The configuration has been saved</b>", Window.Notification.TYPE_HUMANIZED_MESSAGE);
				notifyEventoToControllerWithData(NewUSCUAssociationView.EVENT_SAVE_USCU_ASSOC_VIEW, event.getSource(), selectedInstance);

			}
				////
	
		});
		
		
		/// attribute
		// ----------attribute config
		impl.getTableAttributeConfig().addListener((ItemClickListener) this);
		impl.getTableAttributeConfig().setSelectable(true);
		impl.getTableAttributeConfig().setImmediate(true);
		impl.getTableAttributeConfig().setReadThrough(true);
		impl.getTableAttributeConfig().setMultiSelect(false);
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
					modified ++;
				}
			}

		});
		impl.getButtonAddAttributeConfig().addListener(new ClickListener() {
			@SuppressWarnings("unchecked")
			public void buttonClick(ClickEvent event) {
					Item item = impl.getTableAttributeConfig().addItem(new Integer(idx++));
					item.getItemProperty(ATTRIBUTE_NAME).setValue(ATTRIBUTE_NAME);
					item.getItemProperty(ATTRIBUTE_MAX_VALUE).setValue(ATTRIBUTE_MAX_VALUE);
					item.getItemProperty(ATTRIBUTE_MIN_VALUE).setValue(ATTRIBUTE_MIN_VALUE);
					item.getItemProperty(ATTRIBUTE_DEFAULT_VALUE).setValue(ATTRIBUTE_DEFAULT_VALUE);
			}
		});

		impl.getButtonRemoveAttributeConfig().addListener(new ClickListener() {
			public void buttonClick(ClickEvent event) {
				Set<Integer> c = (Set<Integer>) impl.getTableAttributeConfig().getValue();
				for (Integer idx : c) {
					impl.getTableAttributeConfig().removeItem(idx);
				}
				modified ++;
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
		Vector<DatasetAttribute> datattr=null;
		if (sourceData instanceof ViewNotifyEvent) {
			modified =0;
			viewEvent = (ViewNotifyEvent) sourceData;
			
			if (viewEvent.getEventKind().equals(NewUSCUAssociationView.SET_SC_CUTYPE_VALUE)) {
				impl.getUsSelected().setValue(((String[]) viewEvent.getEventData())[0]);
				//impl.getCuTypeSelected().setValue(((String[]) viewEvent.getEventData())[1]);
			} else if (viewEvent.getEventKind().equals(NewUSCUAssociationView.SET_ASSOCIATION_TO_EDIT)) {
				associationToEdit =  (UnitServerCuInstance) viewEvent.getEventData();
				impl.getUsSelected().setValue(associationToEdit.getUnitServerAlias());
				//impl.getCuTypeSelected().setValue(associationToEdit.getCuType());
				impl.getCuIDTextField().setValue(associationToEdit.getCuId());
				impl.getCuParamTextArea().setValue(associationToEdit.getCuParam());
				impl.getCheckBoxAutoLoad().setValue(associationToEdit.getAutoLoad());
				
				for (DriverSpec driverSpec : associationToEdit.fillDriverVectorFromBSONDescription()) {
					Item woItem = impl.getTableDriverSpecifications().addItem(new Integer(index++));
					woItem.getItemProperty(NewUSCUAssociationView.TABLE_COLUMN_DRIVER_ALIAS).setValue(driverSpec.getDriverName());
					woItem.getItemProperty(NewUSCUAssociationView.TABLE_COLUMN_DRIVER_VERSION).setValue(driverSpec.getDriverVersion());
					woItem.getItemProperty(NewUSCUAssociationView.TABLE_COLUMN_DRIVER_INIT).setValue(driverSpec.getDriverInit());	
				}
				notifyEventoToControllerWithData(EVENT_LOAD_INSTANCE_ATTRIBUTE,impl.getTableAttributeConfig(), associationToEdit);
				
			} else if(viewEvent.getEventKind().equals(NewUSCUAssociationView.EVENT_LOAD_INSTANCE_ATTRIBUTE)){
					
				datattr = (Vector<DatasetAttribute>) viewEvent.getEventData();
					
			}
			if (datattr == null)
				return;
			impl.getTableAttributeConfig().removeAllItems();
			for (DatasetAttribute da : datattr) {
				Item woItem = impl.getTableAttributeConfig().addItem(new Integer(idx++));
				woItem.getItemProperty(NewUSCUAssociationView.ATTRIBUTE_NAME).setValue(da.getName());
				woItem.getItemProperty(NewUSCUAssociationView.ATTRIBUTE_DEFAULT_VALUE).setValue(da.getDefaultValue());
				woItem.getItemProperty(NewUSCUAssociationView.ATTRIBUTE_MAX_VALUE).setValue(da.getRangeMax());
				woItem.getItemProperty(NewUSCUAssociationView.ATTRIBUTE_MIN_VALUE).setValue(da.getRangeMin());
			}
		}
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.vaadin.data.Container.PropertySetChangeListener#containerPropertySetChange(com.vaadin.data.Container.PropertySetChangeEvent)
	 */
	public void containerPropertySetChange(com.vaadin.data.Container.PropertySetChangeEvent event) {
		//System.out.println(event);

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

		if (event.getSource().equals(impl.getTableDriverSpecifications())) {
			driverSpecSelected = event.getItemId();
		}
	}

}
