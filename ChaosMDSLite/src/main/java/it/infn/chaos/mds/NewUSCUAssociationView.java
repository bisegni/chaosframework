/**
 * 
 */
package it.infn.chaos.mds;

import it.infn.chaos.mds.business.DriverSpec;
import it.infn.chaos.mds.business.UnitServerCuInstance;

import java.util.Collection;
import java.util.Observable;

import org.ref.common.mvc.ViewNotifyEvent;
import org.ref.server.webapp.RefVaadinBasePanel;
import org.ref.server.webapp.dialog.RefVaadinErrorDialog;

import com.vaadin.data.Item;
import com.vaadin.event.Action;
import com.vaadin.event.ItemClickEvent;
import com.vaadin.event.ItemClickEvent.ItemClickListener;
import com.vaadin.ui.Button.ClickEvent;
import com.vaadin.ui.Button.ClickListener;
import com.vaadin.ui.Table;

/**
 * @author bisegni
 * 
 */
public class NewUSCUAssociationView extends RefVaadinBasePanel implements com.vaadin.data.Container.PropertySetChangeListener, ItemClickListener {
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
	private static final Object			TABLE_COLUMN_DRIVER_ALIAS		= "Name";
	private static final Object			TABLE_COLUMN_DRIVER_VERSION		= "Version";
	private static final Object			TABLE_COLUMN_DRIVER_INIT		= "Init Param";
	private NewUSCUAssociationVaadin	impl							= new NewUSCUAssociationVaadin();
	private boolean						editingServer					= false;
	private Object						driverSpecSelected				= null;
	private int							index							= 0;
	private UnitServerCuInstance		associationToEdit				= null;

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
				notifyEventoToControllerWithData(NewUSCUAssociationView.EVENT_CANCEL_USCU_ASSOC_VIEW, event.getSource(), null);
			}
		});

		impl.getButtonSave().addListener(new ClickListener() {
			public void buttonClick(ClickEvent event) {
				UnitServerCuInstance usci = associationToEdit == null ? new UnitServerCuInstance():associationToEdit;
				try {
					usci.setUnitServerAlias(impl.getUsSelected().getValue().toString());
					usci.setCuId(impl.getCuIDTextField().getValue().toString());
					usci.setCuType(impl.getCuTypeSelected().getValue().toString());
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
					notifyEventoToControllerWithData(NewUSCUAssociationView.EVENT_SAVE_USCU_ASSOC_VIEW, event.getSource(), usci);
				} catch (Throwable e) {
					RefVaadinErrorDialog.showError(getWindow(), "Save Assocaition", "Some value are not good");

				}
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
			if (viewEvent.getEventKind().equals(NewUSCUAssociationView.SET_SC_CUTYPE_VALUE)) {
				impl.getUsSelected().setValue(((String[]) viewEvent.getEventData())[0]);
				impl.getCuTypeSelected().setValue(((String[]) viewEvent.getEventData())[1]);
			} else if (viewEvent.getEventKind().equals(NewUSCUAssociationView.SET_ASSOCIATION_TO_EDIT)) {
				associationToEdit =  (UnitServerCuInstance) viewEvent.getEventData();
				impl.getUsSelected().setValue(associationToEdit.getUnitServerAlias());
				impl.getCuTypeSelected().setValue(associationToEdit.getCuType());
				impl.getCuIDTextField().setValue(associationToEdit.getCuId());
				impl.getCuParamTextArea().setValue(associationToEdit.getCuParam());
				impl.getCheckBoxAutoLoad().setValue(associationToEdit.getAutoLoad());
				
				for (DriverSpec driverSpec : associationToEdit.fillDriverVectorFromBSONDescription()) {
					Item woItem = impl.getTableDriverSpecifications().addItem(new Integer(index++));
					woItem.getItemProperty(NewUSCUAssociationView.TABLE_COLUMN_DRIVER_ALIAS).setValue(driverSpec.getDriverName());
					woItem.getItemProperty(NewUSCUAssociationView.TABLE_COLUMN_DRIVER_VERSION).setValue(driverSpec.getDriverVersion());
					woItem.getItemProperty(NewUSCUAssociationView.TABLE_COLUMN_DRIVER_INIT).setValue(driverSpec.getDriverInit());	
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
