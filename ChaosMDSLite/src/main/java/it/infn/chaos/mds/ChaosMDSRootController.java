package it.infn.chaos.mds;

import it.infn.chaos.mds.batchexecution.DeinitCU;
import it.infn.chaos.mds.batchexecution.GetCUState;
import it.infn.chaos.mds.batchexecution.InitCU;
import it.infn.chaos.mds.batchexecution.StartCU;
import it.infn.chaos.mds.batchexecution.StopCU;
import it.infn.chaos.mds.batchexecution.SystemCommandWorkUnit;
import it.infn.chaos.mds.batchexecution.UnitServerACK;
import it.infn.chaos.mds.batchexecution.SystemCommandWorkUnit.LoadUnloadWorkUnitSetting;
import it.infn.chaos.mds.business.DataServer;
import it.infn.chaos.mds.business.Dataset;
import it.infn.chaos.mds.business.DatasetAttribute;
import it.infn.chaos.mds.business.Device;
import it.infn.chaos.mds.business.DeviceClass;
import it.infn.chaos.mds.business.UnitServer;
import it.infn.chaos.mds.business.UnitServerCuInstance;
import it.infn.chaos.mds.da.DeviceDA;
import it.infn.chaos.mds.event.EventsToVaadin;
import it.infn.chaos.mds.process.ManageDeviceProcess;
import it.infn.chaos.mds.process.ManageServerProcess;
import it.infn.chaos.mds.process.ManageUnitServerProcess;
import it.infn.chaos.mds.process.MetaDataServerProcess;

import java.io.ByteArrayInputStream;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.PrintWriter;
import java.io.UnsupportedEncodingException;
import java.net.URL;
import java.sql.SQLException;
import java.util.Collection;
import java.util.Date;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Observable;
import java.util.Set;
import java.util.Vector;

import org.bson.BasicBSONObject;
import org.ref.common.exception.RefException;
import org.ref.common.mvc.ViewNotifyEvent;
import org.ref.server.auth.RefAuthneticatorListener;
import org.ref.server.webapp.RefVaadinApplicationController;
import org.ref.server.webapp.RefVaadinMenuManager;
import org.ref.server.webapp.dialog.RefVaadinErrorDialog;

import com.vaadin.Application;
import com.vaadin.data.Item;
import com.vaadin.data.Property.ConversionException;
import com.vaadin.data.Property.ReadOnlyException;
import com.vaadin.data.util.BeanContainer;
import com.vaadin.terminal.DownloadStream;
import com.vaadin.terminal.ExternalResource;
import com.vaadin.terminal.FileResource;
import com.vaadin.terminal.Resource;
import com.vaadin.terminal.URIHandler;
import com.vaadin.terminal.gwt.server.WebApplicationContext;
import com.vaadin.ui.AbstractComponent;
import com.vaadin.ui.Component;
import com.vaadin.ui.Table;
import com.vaadin.ui.Window;
import com.vaadin.ui.UriFragmentUtility.FragmentChangedEvent;
import com.vaadin.ui.Window.Notification;

@SuppressWarnings("serial")
public class ChaosMDSRootController extends RefVaadinApplicationController implements RefAuthneticatorListener {
	private ManageDeviceProcess		mdp			= null;
	private ManageServerProcess		msp			= null;
	private ManageUnitServerProcess	musp		= null;
	private MetaDataServerProcess	mds			= null;

	private String					unitServerSelected;
	private String					unitServerCUTypeSelected;
	private UnitServerCuInstance	cuselected	= null;
	private Application				webapp		= null;
	private Window errwin = null;

	/*
	 * (non-Javadoc)
	 * 
	 * @see org.ref.server.webapp.RefVaadinApplicationController#initController()
	 */
	@Override
	public void initController() throws Throwable {
		mdp = (ManageDeviceProcess) openProcess(ManageDeviceProcess.class.getSimpleName(), null);
		msp = (ManageServerProcess) openProcess(ManageServerProcess.class.getSimpleName(), mdp.getProcessName());
		musp = (ManageUnitServerProcess) openProcess(ManageUnitServerProcess.class.getSimpleName(), mdp.getProcessName());
		mds = (MetaDataServerProcess) openProcess(MetaDataServerProcess.class.getSimpleName(), null);
		
		File init = new File("mds_init.conf");
		openViewByKeyAndClass("VISTA", MDSAppView.class);
		EventsToVaadin.getInstance().addListener(this);
		MDSAppView view = getViewByKey("VISTA");
		
		if(init.exists()){
			
			try {
				File dest = new File("mds_init.conf.loaded");
				mds.applyConfig("mds_init.conf", 0);
				view.getWindow().showNotification("mds_init.conf successfully loaded");
				init.renameTo(dest);
		
			} catch ( java.sql.SQLException e){
				System.out.println("closing and reopen SQL ");
				msp.close();
				mds.close();
				mdp.close();
				musp.close();
				mdp = (ManageDeviceProcess) openProcess(ManageDeviceProcess.class.getSimpleName(), null);
				msp = (ManageServerProcess) openProcess(ManageServerProcess.class.getSimpleName(), mdp.getProcessName());
				musp = (ManageUnitServerProcess) openProcess(ManageUnitServerProcess.class.getSimpleName(), mdp.getProcessName());
				mds = (MetaDataServerProcess) openProcess(MetaDataServerProcess.class.getSimpleName(), null);
			} catch (Throwable e){
				File dest = new File("mds_init.conf.error");

				view.getWindow().showNotification("Error loading \"mds_init.conf\": "+e.getMessage(),Notification.TYPE_ERROR_MESSAGE);
				init.renameTo(dest);

				e.printStackTrace();
			}
		}

		
		URIHandler uriHandler = new URIHandler() {

			@Override
			public DownloadStream handleURI(URL context, String relativeUri) {
				if (relativeUri.equals("test.txt")) {
					DownloadStream ds = new DownloadStream(new ByteArrayInputStream("test".getBytes()), "text/plain", "test.txt");
					ds.setParameter("Content-Disposition", "attachment; filename=test.txt");
					return ds;
				}
				return null;
			}

		};
		Vector<UnitServerCuInstance> instanceForUnitServer = null;
		List<UnitServer> allUnitServer = musp.getAllUnitServer();
		for (UnitServer us : allUnitServer) {
			instanceForUnitServer = musp.loadAllAssociationForUnitServerAlias(us.getAlias());
			for(UnitServerCuInstance i:instanceForUnitServer){
				mdp.deleteDeviceByInstance(i.getCuId());
			}
		}
	
		
		AbstractComponent ac = (AbstractComponent) getViewByKey("VISTA");
		ac.getWindow().addURIHandler(uriHandler);

		webapp = ac.getWindow().getApplication();
		webapp.setTheme("chaosmdslitetheme");

	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see org.ref.server.webapp.RefVaadinApplicationController#updateMainView(org .ref.server.webapp .RefVaadinApplicationController, com.vaadin.ui.Component)
	 */
	@Override
	public void updateMainView(RefVaadinApplicationController controller, Component applicationViewComponent) {
		try {
			if (applicationViewComponent.equals(getViewByKey("VISTA"))) {
				updateUnitServerList();
				updateDeviceList(false);
			} else if (applicationViewComponent.equals(getViewByKey("VISTA_DUE"))) {
				notifyEventoToViewWithData(LiveDataPreferenceView.EVENT_PREFERENCE_UPDATE_SERVER_LIST, this, msp.getAllServer());
			}
		} catch (Throwable e) {
			e.printStackTrace();
		}
	}

	/**
	 * 
	 */
	public void updateMainView(String key, RefVaadinApplicationController controller, Component applicationViewComponent) {
		updateMainView(controller, applicationViewComponent);
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see org.ref.server.webapp.RefVaadinApplicationController#update(java.util .Observable, java.lang.Object)
	 */
	@Override
	public void update(Observable source, Object sourceData) {
		errwin = null;
		if (sourceData == null)
			return;
		try {
			ViewNotifyEvent viewEvent = null;

			if (sourceData instanceof ViewNotifyEvent) {
				viewEvent = (ViewNotifyEvent) sourceData;
				if (viewEvent.getEventSource() instanceof AbstractComponent) {
					AbstractComponent ac = (AbstractComponent) viewEvent.getEventSource();
					errwin = ac.getWindow();
				}
				if (viewEvent.getEventKind().equals(LiveDataPreferenceView.EVENT_PREFERENCE_CLOSE_VIEW)) {
					deleteViewByKey("VISTA_DUE");

				} else if (viewEvent.getEventKind().equals(MDSAppView.EVENT_INITIALIZE_DEVICE_AT_STARTUP)) {
					initializedOptionForSelectedDevice(true);
				} else if (viewEvent.getEventKind().equals(LiveDataPreferenceView.EVENT_PREFERENCE_ADD_NEW_SERVER)) {
					msp.addNewServer();
					notifyEventoToViewWithData(LiveDataPreferenceView.EVENT_PREFERENCE_UPDATE_SERVER_LIST, this, msp.getAllServer());
				} else if (viewEvent.getEventKind().equals(LiveDataPreferenceView.EVENT_PREFERENCE_DELETE_SERVER)) {
					deleteSeletedDataserver();
				} else if (viewEvent.getEventKind().equals(LiveDataPreferenceView.EVENT_PREFERENCE_UPDATE_SERVER_DATA)) {
					updateServerData();
				} else if (viewEvent.getEventKind().equals(MDSAppView.EVENT_DEVICE_SELECTED)) {
					deviceHasBeenSelected(viewEvent.getEventData());
				} else if (viewEvent.getEventKind().equals(MDSAppView.EVENT_UPDATE_DEVICE_LIST)) {
					updateDeviceList(false);
				} else if (viewEvent.getEventKind().equals(MDSAppView.EVENT_DATASET_SELECTED)) {
					updateDatasetAttributeList((Dataset) viewEvent.getEventData());
				} else if (viewEvent.getEventKind().equals(MDSAppView.EVENT_ATTRIBUTE_SAVE)) {
					updateAttributeUserModification();
				} else if (viewEvent.getEventKind().equals(MDSAppView.EVENT_DELETE_SELECTED_DEVICE)) {
					deleteSelectedDevice();
				} else if (viewEvent.getEventKind().equals(MDSAppView.EVENT_SHOW_PREFERENCE)) {
					try {
						openViewByKeyAndClass("VISTA_DUE", LiveDataPreferenceView.class, OpenViewIn.DIALOG, "Live Data Preference");
					} catch (InstantiationException e) {
						e.printStackTrace();
					} catch (IllegalAccessException e) {
						e.printStackTrace();
					}
				} else if (viewEvent.getEventKind().equals(MDSAppView.EVENT_UNIT_SERVER_SELECTED)) {
					unitServerSelected = viewEvent.getEventData().toString();
					unitServerHasBeenSelected(viewEvent.getEventData());
				} else if (viewEvent.getEventKind().equals(MDSAppView.EVENT_UNIT_SERVER_NEW)) {
					createNewUnitServer();
				} else if (viewEvent.getEventKind().equals(MDSAppView.EVENT_UNIT_SERVER_DELETE)) {
					if (unitServerSelected == null) {
						MDSAppView view = getViewByKey("VISTA");
						RefVaadinErrorDialog.showError(view.getWindow(), "Delete unit server", "Select one unit server");
					} else {
						deleteSelectedUnitServer();
					}
				} else if (viewEvent.getEventKind().equals(MDSAppView.EVENT_UNIT_SERVER_CU_TYPE_SELECTED)) {
					unitServerCUTypeSelected = viewEvent.getEventData().toString();
				} else if (viewEvent.getEventKind().equals(MDSAppView.EVENT_UNIT_SERVER_CREATE_US_CU_ASSOCIATION)) {
					try {
						if (unitServerSelected != null) {
							openViewByKeyAndClass("NEW_US_CU_ASSOCIATION", NewUSCUAssociationView.class, OpenViewIn.MAIN_VIEW, "New Unit server - Control Unit Association");

							updateDeviceClassTable(unitServerSelected);
							notifyEventoToViewWithData(NewUSCUAssociationView.SET_SC_CUTYPE_VALUE, this, unitServerSelected);
						} else {
							MDSAppView view = getViewByKey("VISTA");
							RefVaadinErrorDialog.showError(view.getWindow(), "SC-CU Association error", "SC or CU type invalid association");
						}
					} catch (InstantiationException e) {
						e.printStackTrace();
					} catch (IllegalAccessException e) {
						e.printStackTrace();
					}
				} else if (viewEvent.getEventKind().equals(MDSUIEvents.EVENT_SAVE_USCU_ASSOC_VIEW)) {
					deleteViewByKey("NEW_US_CU_ASSOCIATION");
					cuselected = (UnitServerCuInstance) viewEvent.getEventData();
					Vector<String> ret = musp.getCuTypeForUnitServer(cuselected.getUnitServerAlias());

					if ((ret == null) || (ret.contains(cuselected.getCuType()) == false)) {
						// add if not found
						musp.addCuTypeToUnitServerAlias(cuselected.getUnitServerAlias(), cuselected.getCuType());
					}
					musp.saveUSCUAssociation(cuselected);
					saveAssociationAttributeConfig(cuselected);
					unitServerHasBeenSelected(cuselected.getUnitServerAlias());
					openViewByKeyAndClass("VISTA", MDSAppView.class);

				} else if (viewEvent.getEventKind().equals(MDSUIEvents.EVENT_COPY_CU)) {
					Date date = new Date();

					cuselected = (UnitServerCuInstance) viewEvent.getEventData();
					cuselected.setAttributeConfigutaion(musp.loadAllAssociationAttributeConfigForUnitServerAlias(cuselected));
					String newname = cuselected.getCuId() + "_" + date.getTime();
					cuselected.setCuId(newname);
					cuselected.cuIdModified(false);
					musp.saveUSCUAssociation(cuselected);
					saveAssociationAttributeConfig(cuselected);
					unitServerHasBeenSelected(cuselected.getUnitServerAlias());

				} else if (viewEvent.getEventKind().equals(MDSUIEvents.EVENT_EDIT_DEVICE_CLASS)) {

					openViewByKeyAndClass("NEW_DEVICE_CLASS", DeviceClassListView.class, OpenViewIn.MAIN_VIEW, "New/Edit Device Class");

					updateDeviceClassTable(null);

				} else if (viewEvent.getEventKind().equals(MDSUIEvents.EVENT_REFRESH_STATE)) {
					// refreshedUS
					// updateUnitServerList();
					// updateDeviceList();
					getUnitState();
					refreshUnitServer(null);
					updateDeviceList(true);
				} else if (viewEvent.getEventKind().equals(MDSUIEvents.CHAOS_EVENT_US_UPDATE)) {
					refreshUnitServer((UnitServer) viewEvent.getEventData());
				} else if (viewEvent.getEventKind().equals(MDSUIEvents.EVENT_REMOVE_DEVICE_CLASS)) {

					DeviceClass dc = (DeviceClass) viewEvent.getEventData();
					addRemoveDeviceClass(true, viewEvent.getEventData());
					if (cuselected != null) {
						musp.removeCuTypeToUnitServer(cuselected.getUnitServerAlias(), cuselected.getUnitServerAlias());
					} else {
						musp.removeCuTypeToUnitServer(null, dc.getDeviceClass());

					}
				} else if (viewEvent.getEventKind().equals(MDSUIEvents.EVENT_SAVE_DEVICE_CLASS)) {
					// musp.addCuTypeToUnitServerAlias(cuselected.getUnitServerAlias(), cuselected.getCuType());

					addRemoveDeviceClass(false, viewEvent.getEventData());

				} else if (viewEvent.getEventKind().equals(MDSUIEvents.EVENT_CLOSE_DEVICE_CLASS_VIEW)) {
					deleteViewByKey("NEW_DEVICE_CLASS");
					openViewByKeyAndClass("NEW_US_CU_ASSOCIATION", NewUSCUAssociationView.class, OpenViewIn.MAIN_VIEW, "New/Edit Unit server - Control Unit Association");
					updateDeviceClassTable(null);
				} else if (viewEvent.getEventKind().equals(MDSUIEvents.EVENT_CANCEL_USCU_ASSOC_VIEW)) {
					deleteViewByKey("NEW_US_CU_ASSOCIATION");
					openViewByKeyAndClass("VISTA", MDSAppView.class);
				} else if (viewEvent.getEventKind().equals(MDSUIEvents.EVENT_DUMP_CONFIGURATION)) {
					dumpConfiguration();
					deleteViewByKey("VISTA_DUE");
					openViewByKeyAndClass("VISTA", MDSAppView.class);
				} else if (viewEvent.getEventKind().equals(MDSUIEvents.EVENT_APPLY_CONFIGURATION)) {
					if (viewEvent.getEventData() != null) {
						Vector<Object> obj = (Vector<Object>) viewEvent.getEventData();
						try{
							applyConfiguration(obj);
							deleteViewByKey("VISTA_DUE");
							openViewByKeyAndClass("VISTA", MDSAppView.class);
							MDSAppView view = getViewByKey("VISTA");

							view.getWindow().showNotification("Configuration successfully applied");
							updateUnitServerList();
							updateDeviceList(true);
							unitServerHasBeenSelected(null);
						} catch  (Throwable e ){
							LiveDataPreferenceView  view = getViewByKey("VISTA_DUE");

							view.getWindow().showNotification("Error processing \""+obj.get(0).toString() +"\":"+e.getMessage(), Notification.TYPE_ERROR_MESSAGE);
						}
					}
					
				} else if (viewEvent.getEventKind().equals(MDSAppView.EVENT_UNIT_SERVER_LOAD_ALL_ASSOCIATION)) {
					if (unitServerSelected != null) {
						loadUnloadAllUnitServerAssociation(true);
					} else {
						MDSAppView view = getViewByKey("VISTA");
						RefVaadinErrorDialog.showError(view.getWindow(), "Load all association", "A unit server need to be selected");
					}
				} else if (viewEvent.getEventKind().equals(MDSAppView.EVENT_UNIT_SERVER_UNLOAD_ALL_ASSOCIATION)) {
					if (unitServerSelected != null) {
						loadUnloadAllUnitServerAssociation(false);
					} else {
						MDSAppView view = getViewByKey("VISTA");
						RefVaadinErrorDialog.showError(view.getWindow(), "Unload all association", "A unit server need to be selected");
					}
				} else if (viewEvent.getEventKind().equals(MDSUIEvents.EVENT_UPDATE_LIST)) {
					notifyEventoToViewWithData(MDSUIEvents.EVENT_UPDATE_LIST, this, musp.loadAllAssociationForUnitServerAlias(unitServerSelected));
				} else if (viewEvent.getEventKind().equals(MDSUIEvents.EVENT_REMOVE_ASSOCIATION)) {
					Set<UnitServerCuInstance> vcui = new HashSet<UnitServerCuInstance>();
					vcui.add((UnitServerCuInstance) viewEvent.getEventData());
					removeAssociation(vcui);
				} else if (viewEvent.getEventKind().equals(MDSUIEvents.EVENT_LOAD_INSTANCE)) {
					loadUnloadInstance((Set<UnitServerCuInstance>) viewEvent.getEventData(), true);
				} else if (viewEvent.getEventKind().equals(MDSUIEvents.EVENT_UNLOAD_INSTANCE)) {
					loadUnloadInstance((Set<UnitServerCuInstance>) viewEvent.getEventData(), false);
				} else if (viewEvent.getEventKind().equals(MDSUIEvents.EVENT_SAVE_ATTRIBUTE_CONFIG)) {
					saveAssociationAttributeConfig((UnitServerCuInstance) viewEvent.getEventData());
				} else if (viewEvent.getEventKind().equals(MDSUIEvents.EVENT_LOAD_INSTANCE_ATTRIBUTE)) {
					loadAssociationAttributeConfig((UnitServerCuInstance) viewEvent.getEventData());
				} else if (viewEvent.getEventKind().equals(MDSUIEvents.EVENT_CLOSE_VIEW)) {
					deleteViewByKey("US_CU_ASSOCIATION_LIST");
					openViewByKeyAndClass("VISTA", MDSAppView.class);
				} else if (viewEvent.getEventKind().equals(MDSUIEvents.EVENT_EDIT_ASSOCIATION)) {
					// Set<UnitServerCuInstance> associationToEdit = (Set<UnitServerCuInstance>) viewEvent.getEventData();

					// if (associationToEdit.size() == 1) {
					cuselected = (UnitServerCuInstance) viewEvent.getEventData();
					openViewByKeyAndClass("NEW_US_CU_ASSOCIATION", NewUSCUAssociationView.class, OpenViewIn.MAIN_VIEW, "New/Edit Unit server - Control Unit Association");
					notifyEventoToViewWithData(NewUSCUAssociationView.SET_ASSOCIATION_TO_EDIT, this, cuselected);
					updateDeviceClassTable(cuselected.getUnitServerAlias());

				} else if (viewEvent.getEventKind().equals(MDSAppView.EVENT_UNIT_SERVER_EDIT_ALIAS)) {
					openViewByKeyAndClass("EDIT_US_ALIAS", USEditInfoView.class, OpenViewIn.MAIN_VIEW, "Edit Unit Server Property");
					notifyEventoToViewWithData(USEditInfoView.EVENT_SET_UNIT_SERVER, this, musp.getUnitServerByIdentification(viewEvent.getEventData().toString()));
				} else if (viewEvent.getEventKind().equals(USEditInfoView.EVENT_CANCELL)) {
					deleteViewByKey("EDIT_US_ALIAS");
					openViewByKeyAndClass("VISTA", MDSAppView.class);
				} else if (viewEvent.getEventKind().equals(USEditInfoView.EVENT_SAVE)) {
					musp.updaUnitServerProperty((UnitServer) viewEvent.getEventData());
					updateUnitServerList();
					deleteViewByKey("EDIT_US_ALIAS");
					openViewByKeyAndClass("VISTA", MDSAppView.class);
				} else if (viewEvent.getEventKind().equals(MDSAppView.EVENT_UPDATE_UNIT_SERVER_LIST) || viewEvent.getEventKind().equals(MDSUIEvents.EVENT_UI_REGISTERED)) {
					updateUnitServerList();
				} else if (viewEvent.getEventKind().equals(MDSAppView.EVENT_UNIT_SERVER_CU_TYPE_ADD)) {
					openViewByKeyAndClass("US_CU_TYPE_ADD", USAddCuTypeView.class, OpenViewIn.DIALOG, "Add unit type");
					notifyEventoToViewWithData(USAddCuTypeView.EVENT_SET_UNIT_SERVER, this, viewEvent.getEventData().toString());
				} else if (viewEvent.getEventKind().equals(USAddCuTypeView.EVENT_CANCELL)) {
					deleteViewByKey("US_CU_TYPE_ADD");
				} else if (viewEvent.getEventKind().equals(USAddCuTypeView.EVENT_SAVE)) {
					String[] data = (String[]) viewEvent.getEventData();
					musp.addCuTypeToUnitServerAlias(data[0], data[1]);
					unitServerHasBeenSelected(data[0]);
					deleteViewByKey("US_CU_TYPE_ADD");
				} else if (viewEvent.getEventKind().equals(MDSAppView.EVENT_UNIT_SERVER_CU_TYPE_REMOVE)) {
					if (unitServerSelected != null) {
						musp.removeCuTypeToUnitServer(unitServerSelected, viewEvent.getEventData().toString());
						unitServerHasBeenSelected(unitServerSelected);
					} else {
						MDSAppView view = getViewByKey("VISTA");
						RefVaadinErrorDialog.showError(view.getWindow(), "Remove Unit Server Type", "A unit server need to be selected");
					}
				} else if (viewEvent.getEventKind().equals(MDSAppView.EVENT_UNIT_SERVER_CU_TYPE_UPDATE_LIST)) {
					if (unitServerSelected != null) {
						unitServerHasBeenSelected(viewEvent.getEventData().toString());
					} else {
						MDSAppView view = getViewByKey("VISTA");
						RefVaadinErrorDialog.showError(view.getWindow(), "Remove Unit Server Type", "A unit server need to be selected");
					}
				} else if (viewEvent.getEventKind().equals(MDSUIEvents.EVENT_SWITCH_AUTOLOAD)) {
					musp.switcAutoloadOptionOnAssociation((Set<UnitServerCuInstance>) viewEvent.getEventData());
					notifyEventoToViewWithData(MDSUIEvents.EVENT_UPDATE_LIST, this, musp.loadAllAssociationForUnitServerAlias(unitServerSelected));
				} else if (viewEvent.getEventKind().equals(USEditInfoView.EVENT_DELETE_SECURITY_KEY)) {
					musp.deleteSecurityKeys((UnitServer) viewEvent.getEventData());
					notifyEventoToViewWithData(USEditInfoView.EVENT_SET_UNIT_SERVER, this, musp.getUnitServerByIdentification(((UnitServer) viewEvent.getEventData()).getAlias()));
				} else if (viewEvent.getEventKind().equals(MDSUIEvents.EVENT_CU_REGISTERED)) {
					updateDeviceList(false);
				} else if (viewEvent.getEventKind().equals(MDSUIEvents.EVENT_REMOVE_CU_DEVICES)) {
					Set<UnitServerCuInstance> usv = new HashSet<UnitServerCuInstance>();
					if (viewEvent.getEventData() == null)
						return;
					usv.add((UnitServerCuInstance) viewEvent.getEventData());
					removeDeviceAssociated(usv);
				} else if (viewEvent.getEventKind().equals(MDSUIEvents.EVENT_NODE_START)) {
					Device devInfo = (Device) viewEvent.getEventData();
					if (devInfo != null) {
						SingletonServices.getInstance().getSlowExecution().submitJob(StartCU.class.getName(), devInfo);
					}
				} else if (viewEvent.getEventKind().equals(MDSUIEvents.EVENT_NODE_STOP)) {
					Device devInfo = (Device) viewEvent.getEventData();
					if (devInfo != null) {
						SingletonServices.getInstance().getSlowExecution().submitJob(StopCU.class.getName(), devInfo);
					}
				} else if (viewEvent.getEventKind().equals(MDSUIEvents.EVENT_NODE_INIT)) {
					Object[] initData = new Object[2];
					Device devInfo = (Device) viewEvent.getEventData();
					devInfo = musp.getDeviceFromUniqueID(devInfo.getDeviceIdentification());
					if (devInfo != null) {
						initData[0] = devInfo;
						initData[1] = msp.getAllServer();
						SingletonServices.getInstance().getSlowExecution().submitJob(InitCU.class.getName(), initData);
					}
				} else if (viewEvent.getEventKind().equals(MDSUIEvents.EVENT_NODE_DEINIT)) {
					Device devInfo = (Device) viewEvent.getEventData();
					if (devInfo != null) {
						SingletonServices.getInstance().getSlowExecution().submitJob(DeinitCU.class.getName(), devInfo);
					}
				}

			}
		} catch (Throwable e) {
			// MDSAppView view = getViewByKey("VISTA");
			// Window win = view.getWindow();
		/*	if ((errwin != null) && e.getMessage()!=null) {
				RefVaadinErrorDialog.showError(errwin, "Internal Error:", "Stack Trace:" + e.getMessage());
			}*/
			e.printStackTrace();
		}
	}

	/**
	 * @param v
	 * @throws Throwable
	 */
	private void applyConfiguration(Vector<Object> v) throws Throwable {
		// TODO Auto-generated method stub
	
			String configname = (String) v.get(0);
			Integer replace = (Integer) v.get(1);
		
			mds.applyConfig(configname, replace);
		
	}

	/**
	 * 
	 * @param eventData
	 * @throws InstantiationException
	 * @throws IllegalAccessException
	 * @throws ClassNotFoundException
	 * @throws RefException
	 */
	private void loadAssociationAttributeConfig(UnitServerCuInstance eventData) throws InstantiationException, IllegalAccessException, ClassNotFoundException, RefException {
		notifyEventoToViewWithData(MDSUIEvents.EVENT_LOAD_INSTANCE_ATTRIBUTE, this, musp.loadAllAssociationAttributeConfigForUnitServerAlias(eventData));
	}

	/**
	 * 
	 * @param eventData
	 * @throws InstantiationException
	 * @throws IllegalAccessException
	 * @throws ClassNotFoundException
	 * @throws SQLException
	 * @throws RefException
	 */
	private void saveAssociationAttributeConfig(UnitServerCuInstance eventData) throws InstantiationException, IllegalAccessException, ClassNotFoundException, SQLException, RefException {
		musp.saveAllAttributeConfigForAssociation(eventData);
	}

	/**
	 * 
	 * @param loadUnload
	 * @throws Throwable
	 */
	private void loadUnloadAllUnitServerAssociation(boolean loadUnload) throws Throwable {
		Vector<UnitServerCuInstance> instanceForUnitServer = musp.loadAllAssociationForUnitServerAlias(unitServerSelected);
		Set<UnitServerCuInstance> instanceSet = new HashSet<UnitServerCuInstance>();
		instanceSet.addAll(instanceForUnitServer);
		loadUnloadInstance(instanceSet, loadUnload);
	}

	private void removeDeviceAssociated(Set<UnitServerCuInstance> ass) throws Throwable {
		for (UnitServerCuInstance association : ass) {
			mdp.deleteDeviceByInstance(association.getCuId());
		}
		updateDeviceList(false);
	}

	private void dumpConfiguration() throws Throwable {
		try {
			WebApplicationContext webctx = (WebApplicationContext) webapp.getContext();
			String sid = webctx.getHttpSession().getId();
			String mypath = webctx.getBaseDirectory().getAbsolutePath();
			String filename = mypath + "/MDSConfig_" + sid + ".txt";
			mds.dumpConfig(filename);

			notifyEventoToViewWithData(MDSUIEvents.CTRL_CONFIG_GENERATED, this, filename);

		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (UnsupportedEncodingException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

		// WebApplicationContext ctx = (WebApplicationContext) getApplication().getContext();
		// String path = ctx.getHttpSession().getServletContext().getContextPath();
		// event.getButton().getWindow().open(new ExternalResource(path + "/test.txt"));
	}

	/**
	 * 
	 * @param eventData
	 * @param b
	 * @throws Throwable
	 */
	private void loadUnloadInstance(Set<UnitServerCuInstance> eventData, boolean loadUnload) throws Throwable {
		LoadUnloadWorkUnitSetting setting = new LoadUnloadWorkUnitSetting();
		setting.unit_server_container = musp.getUnitServerByIdentification(unitServerSelected);
		setting.associations = eventData;
		setting.loadUnload = loadUnload;
		SingletonServices.getInstance().getSlowExecution().submitJob(SystemCommandWorkUnit.class.getName(), setting);
		if (loadUnload == false) {
			UnitServer us = musp.getUnitServerByIdentification(unitServerSelected);
			removeDeviceAssociated(eventData);
		}
	}

	/**
	 * 
	 * @param eventData
	 * @param b
	 * @throws Throwable
	 */
	private void getUnitState() throws Throwable {
		List<UnitServer> allUnitServer = musp.getAllUnitServer();
		for (UnitServer us : allUnitServer) {
			BasicBSONObject actionData = new BasicBSONObject();
			if (us.getIp_port() != null) {
				actionData.append(RPCConstants.CONTROL_UNIT_INSTANCE_NETWORK_ADDRESS, us.getIp_port());
				SingletonServices.getInstance().getSlowExecution().submitJob(GetCUState.class.getName(), actionData);
			}
		}
	}

	/**
	 * 
	 * @param associationToRemove
	 * @throws Throwable
	 */
	private void removeAssociation(Set<UnitServerCuInstance> associationToRemove) throws Throwable {
		if (associationToRemove == null || associationToRemove.size() == 0)
			return;
		musp.removeUSCUAssociation(associationToRemove);
		notifyEventoToViewWithData(MDSUIEvents.EVENT_UPDATE_LIST, this, musp.loadAllAssociationForUnitServerAlias(unitServerSelected));
		removeDeviceAssociated(associationToRemove);

	}

	/**
	 * 
	 * @throws InstantiationException
	 * @throws IllegalAccessException
	 * @throws ClassNotFoundException
	 * @throws SQLException
	 * @throws RefException
	 */
	/*
	 * private void showAllAssociationForUnitServer() throws InstantiationException, IllegalAccessException, ClassNotFoundException, SQLException, RefException { if (unitServerSelected != null) { openViewByKeyAndClass("US_CU_ASSOCIATION_LIST", USCUAssociationListView.class, OpenViewIn.MAIN_VIEW, "Control Unit List"); notifyEventoToViewWithData(USCUAssociationListView.SET_US_ALIAS, this, unitServerSelected); notifyEventoToViewWithData(USCUAssociationListView.EVENT_UPDATE_LIST, this,
	 * musp.loadAllAssociationForUnitServerAlias(unitServerSelected)); } else { MDSAppView view = getViewByKey("VISTA"); RefVaadinErrorDialog.showError(view.getWindow(), "Show all error", "Unit server not selected"); }
	 * 
	 * }
	 */
	private void deleteSelectedUnitServer() throws Throwable {
		if (unitServerSelected != null) {
			musp.deleteUnitServer(unitServerSelected);
			updateUnitServerList();
			Vector<UnitServerCuInstance> cuiv = musp.loadAllAssociationForUnitServerAlias(unitServerSelected);
			for(UnitServerCuInstance c:cuiv){
				mdp.deleteDeviceByInstance(c.getCuId());
			}
			updateDeviceList(false);
		}
	}

	private void createNewUnitServer() throws Throwable {
		musp.createNewUnitServer();
		updateUnitServerList();
	}

	private void refreshUnitServer(UnitServer us) throws Throwable {
		// load all dataset

		if (us == null) {
			List<UnitServer> allUnitServer = musp.getAllUnitServer();
			for (UnitServer uss : allUnitServer) {
				Vector<UnitServerCuInstance> cuiv = musp.loadAllAssociationForUnitServerAlias(uss.getAlias());
				uss.setCuInstances(cuiv);
				notifyEventoToViewWithData(MDSUIEvents.EVENT_REFRESH_US, this, uss);
				notifyEventoToViewWithData(MDSUIEvents.EVENT_REFRESH_LIST, this, uss);
			}
		} else {
			notifyEventoToViewWithData(MDSUIEvents.EVENT_REFRESH_US, this, us);
			notifyEventoToViewWithData(MDSUIEvents.EVENT_REFRESH_LIST, this, us);
		}

	}

	/**
	 * 
	 * @throws Throwable
	 */
	private void updateUnitServerList() throws Throwable {
		// load all dataset
		List<UnitServer> allUnitServer = musp.getAllUnitServer();
		notifyEventoToViewWithData(MDSUIEvents.EVENT_UPDATE_US, this, allUnitServer);
	}

	private void addRemoveDeviceClass(boolean remove, Object dc) throws SQLException, RefException, InstantiationException, IllegalAccessException, ClassNotFoundException {
		DeviceClass obj = (DeviceClass) dc;
		if (obj == null)
			return;

		if (remove == true) {
			musp.removeDeviceClass(obj.getDeviceClass());
		} else {
			musp.insertNewDeviceClass(obj);

		}

		return;
	}

	private void updateDeviceClassTable(String unit_server_selected) throws RefException {

		Vector<DeviceClass> dcv = musp.returnAllClassesBy(unit_server_selected, null, null);
		notifyEventoToViewWithData(MDSUIEvents.EVENT_UPDATE_DEVICE_CLASS, this, dcv);

	}

	/**
	 * A device has been selected so we need to update the datasets
	 * 
	 * @param deviceID
	 * @throws Throwable
	 */
	private void unitServerHasBeenSelected(Object unitServerIdentification) throws Throwable {
		if (unitServerIdentification == null){
			// clean the tab 
			notifyEventoToViewWithData(MDSUIEvents.EVENT_UPDATE_LIST, this, null);
			return;
		}
		UnitServer us = musp.getUnitServerByIdentification(unitServerIdentification.toString());
		Vector<UnitServerCuInstance> cuiv = musp.loadAllAssociationForUnitServerAlias(unitServerIdentification.toString());
		for (UnitServerCuInstance ci : cuiv) {
			// simply fill the device class table with the information provided
			Vector<DeviceClass> dcv = musp.returnAllClassesBy(null, null, ci.getCuType());
			if (dcv != null && (dcv.size() > 0)) {
				ci.setInterface(dcv.get(0).getDeviceClassInterface());
			} else if (dcv.size() == 0) {
				// if not present insert a new class with unknown interface and source 0
				DeviceClass dc = new DeviceClass();
				dc.setDeviceClass(ci.getCuType());
				dc.setDeviceClassAlias(ci.getCuType());
				dc.setDeviceClassInterface("uknown");
				dc.setDeviceSource("pre-existent CU");
				musp.insertNewDeviceClass(dc);
			}
		}
		notifyEventoToViewWithData(MDSUIEvents.EVENT_UPDATE_LIST, this, cuiv);
	}

	/**
	 * @throws Throwable
	 * 
	 */
	private void deleteSeletedDataserver() throws Throwable {
		LiveDataPreferenceView view = getViewByKey("VISTA_DUE");
		Table t = (Table) view.getComponentByKey(LiveDataPreferenceView.KEY_PREFERENCE_TABLE);
		@SuppressWarnings("unchecked")
		Collection<Integer> dataServer = (Collection<Integer>) t.getValue();
		for (Iterator<Integer> iterator = dataServer.iterator(); iterator.hasNext();) {
			Integer dsIDToUpdate = (Integer) iterator.next();
			@SuppressWarnings("unchecked")
			DataServer dsToUpdate = ((BeanContainer<Integer, DataServer>) t.getContainerDataSource()).getItem(dsIDToUpdate).getBean();
			msp.deleteServer(dsToUpdate);
		}
		notifyEventoToViewWithData(LiveDataPreferenceView.EVENT_PREFERENCE_UPDATE_SERVER_LIST, this, msp.getAllServer());

	}

	/**
	 * 
	 * @param initialize
	 * @throws Throwable
	 */
	private void initializedOptionForSelectedDevice(boolean initialize) throws Throwable {
		MDSAppView view = getViewByKey("VISTA");
		Table dsTable = (Table) view.getComponentByKey(MDSAppView.KEY_DEVICE_TAB);
		@SuppressWarnings("unchecked")
		Collection<String> selectedDevice = (Collection<String>) dsTable.getItemIds();
		for (Iterator<String> iterator = selectedDevice.iterator(); iterator.hasNext();) {
			String deviceIdentification = iterator.next();
			mdp.swapDeviceStartupInitizializationOption(deviceIdentification);
		}
		updateDeviceList(false);
	}

	private void updateServerData() throws Throwable {
		LiveDataPreferenceView view = getViewByKey("VISTA_DUE");
		Table t = (Table) view.getComponentByKey(LiveDataPreferenceView.KEY_PREFERENCE_TABLE);
		@SuppressWarnings("unchecked")
		Collection<Integer> dataServer = (Collection<Integer>) t.getItemIds();
		for (Iterator<Integer> iterator = dataServer.iterator(); iterator.hasNext();) {
			Integer dsIDToUpdate = (Integer) iterator.next();
			@SuppressWarnings("unchecked")
			DataServer dsToUpdate = ((BeanContainer<Integer, DataServer>) t.getContainerDataSource()).getItem(dsIDToUpdate).getBean();
			msp.updateDataServer(dsToUpdate);
		}
	}

	/**
	 * @throws Throwable
	 * 
	 */
	private void deleteSelectedDevice() throws Throwable {
		MDSAppView view = getViewByKey("VISTA");
		Table t = (Table) view.getComponentByKey(MDSAppView.KEY_DEVICE_TAB);
		Object o = t.getValue();
		mdp.deleteDeviceByInstance(o.toString());
		updateDeviceList(false);
	}

	/**
	 * @throws RefException
	 */
	private void updateAttributeUserModification() throws RefException {
		MDSAppView view = getViewByKey("VISTA");
		try {
			Table t = (Table) view.getComponentByKey(MDSAppView.KEY_DATASET_ATTRIBUTE_TAB);
			@SuppressWarnings("unchecked")
			Collection<DatasetAttribute> datasetAttributes = (Collection<DatasetAttribute>) t.getItemIds();
			for (DatasetAttribute datasetAttributeItemID : datasetAttributes) {
				Double def = null, max = null, min = null;
				Object tmpValue = null;
				tmpValue = t.getItem(datasetAttributeItemID).getItemProperty(MDSAppView.TAB3_TAGS_PATH);
				if (tmpValue != null) {
					datasetAttributeItemID.setTagsPath(tmpValue.toString());
				}
				tmpValue = t.getItem(datasetAttributeItemID).getItemProperty(MDSAppView.TAB3_RANG_MIN);
				if (tmpValue != null) {
					try {
						min = Double.parseDouble(tmpValue.toString());
					} catch (Throwable e) {
						RefVaadinErrorDialog.showError(view.getWindow(), "Bad MIN:\"" + tmpValue.toString() + "\" format ", e.getMessage());
						notifyEventoToViewWithData(MDSAppView.EVENT_ATTRIBUTE_EDITING, null, null);
						return;
					}
					datasetAttributeItemID.setRangeMin(tmpValue.toString());
				}
				tmpValue = t.getItem(datasetAttributeItemID).getItemProperty(MDSAppView.TAB3_RANG_MAX);
				if (tmpValue != null) {
					try {
						max = Double.parseDouble(tmpValue.toString());
					} catch (Throwable e) {
						RefVaadinErrorDialog.showError(view.getWindow(), "Bad MAX:\"" + tmpValue.toString() + "\" format ", e.getMessage());
						notifyEventoToViewWithData(MDSAppView.EVENT_ATTRIBUTE_EDITING, null, null);
						return;
					}
					datasetAttributeItemID.setRangeMax(tmpValue.toString());
				}
				tmpValue = t.getItem(datasetAttributeItemID).getItemProperty(MDSAppView.TAB3_DEF_VALUE);
				if (tmpValue != null) {

					try {
						def = Double.parseDouble(tmpValue.toString());
					} catch (Throwable e) {
						RefVaadinErrorDialog.showError(view.getWindow(), "Bad Default format ", e.getMessage());
						notifyEventoToViewWithData(MDSAppView.EVENT_ATTRIBUTE_EDITING, null, null);
						return;
					}

					if (max != null && def > max) {
						RefVaadinErrorDialog.showError(view.getWindow(), "DEFAULT: " + def + " is greater than MAX: " + max, "");
						notifyEventoToViewWithData(MDSAppView.EVENT_ATTRIBUTE_EDITING, null, null);
						return;
					}
					if (min != null && def < min) {
						RefVaadinErrorDialog.showError(view.getWindow(), "DEFAULT: " + def + "  is lower than MIN:" + min, "");
						notifyEventoToViewWithData(MDSAppView.EVENT_ATTRIBUTE_EDITING, null, null);
						return;
					}
					datasetAttributeItemID.setDefaultValue(tmpValue.toString());
				}
			}
			mdp.updateDeviceAttributes(datasetAttributes);
		} catch (Throwable e) {
			RefVaadinErrorDialog.showError(view.getWindow(), "Update Attribute Error", e.getMessage());
			notifyEventoToViewWithData(MDSAppView.EVENT_ATTRIBUTE_EDITING, null, null);
		}
	}

	/**
	 * @throws Throwable
	 */
	private void updateDatasetAttributeList(Dataset dsAttr) throws Throwable {
		MDSAppView view = getViewByKey("VISTA");
		Table dsAttrTable = (Table) view.getComponentByKey(MDSAppView.KEY_DATASET_ATTRIBUTE_TAB);
		dsAttrTable.removeAllItems();
		if (dsAttr == null)
			return;
		List<DatasetAttribute> datasetsAttributeForDataset = mdp.getAttributeForDataset(dsAttr);
		for (DatasetAttribute dsAt : datasetsAttributeForDataset) {
			Item woItem = dsAttrTable.addItem(dsAt);
			woItem.getItemProperty(MDSAppView.TAB3_NAME).setValue(dsAt.getName());
			woItem.getItemProperty(MDSAppView.TAB3_DIR).setValue(dsAt.getNamedDirection());
			woItem.getItemProperty(MDSAppView.TAB3_TYPE).setValue(dsAt.getNamedType());
			woItem.getItemProperty(MDSAppView.TAB3_RANG_MIN).setValue(dsAt.getRangeMin());
			woItem.getItemProperty(MDSAppView.TAB3_RANG_MAX).setValue(dsAt.getRangeMax());
			woItem.getItemProperty(MDSAppView.TAB3_DEF_VALUE).setValue(dsAt.getDefaultValue());
			woItem.getItemProperty(MDSAppView.TAB3_TAGS_PATH).setValue(dsAt.getTagsPath());
		}
	}

	/**
	 * A device has been selected so we need to update the datasets
	 * 
	 * @param deviceID
	 * @throws Throwable
	 */
	private void deviceHasBeenSelected(Object deviceIdentification) throws Throwable {
		if (deviceIdentification == null)
			return;
		// visualize just last dataset
		Device dev = (Device) deviceIdentification;
		Dataset ds = mdp.getLastDatasetForDevice(dev.getDeviceIdentification());
		updateDatasetAttributeList(ds);

	}

	/**
	 * @throws Throwable
	 */
	private void updateDeviceList(boolean refresh) throws Throwable {
		deviceHasBeenSelected(null);
		Vector<Device> devlist = new Vector<Device>();
		List<Device> allDevices = mdp.getAllDevices();
		List<UnitServer> allUnitServer = musp.getAllUnitServer();
		for(UnitServer us:allUnitServer){
			List<UnitServerCuInstance> cus = musp.loadAllAssociationForUnitServerAlias(us.getAlias()); 
			for(UnitServerCuInstance cu:cus){
				for(Device d:allDevices){
					if(d.getDeviceIdentification().equals(cu.getCuId())){
						devlist.add(d);
					}
				}
			}
		}
	
		/*
		 * for(Device dev:allDevices){ List<UnitServerCuInstance> cus = musp.loadAllAssociationForUnitServerAlias(us.getAlias()); for(UnitServerCuInstance cu:cus){ if(cu.getCuId() == dev.getDeviceIdentification()){ dev.setParent(cu); dev.setLastDatasetForDevice(mdp.getLastDatasetForDevice(dev.getDeviceIdentification()).getTimestamp()); dev.setAttributes(mdp.getLastDatasetForDevice(dev.getDeviceIdentification()).getAttributes() != null ?
		 * mdp.getLastDatasetForDevice(dev.getDeviceIdentification()).getAttributes().size() : 0);
		 * 
		 * } } } }
		 */
		for (Device dev : devlist) {
			dev.setLastDatasetForDevice(mdp.getLastDatasetForDevice(dev.getDeviceIdentification()).getTimestamp());
			dev.setAttributes(mdp.getLastDatasetForDevice(dev.getDeviceIdentification()).getAttributes() != null ? mdp.getLastDatasetForDevice(dev.getDeviceIdentification()).getAttributes().size() : 0);
		}
		
		if (refresh) {
			notifyEventoToViewWithData(MDSUIEvents.EVENT_REFRESH_DEVICE_LIST, this, devlist);

		} else {
			notifyEventoToViewWithData(MDSUIEvents.EVENT_UPDATE_DEVICE_LIST, this, devlist);
		}

	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see org.ref.server.auth.RefAuthneticatorListener#loggedOut(java.lang.Object[])
	 */
	public void loggedOut(Object... infoData) {
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see org.ref.server.auth.RefAuthneticatorListener#loggedIn(java.lang.Object[])
	 */
	public void loggedIn(Object... infoData) {
	}

	@Override
	public void fragmentChanged(FragmentChangedEvent source) {

	}

	protected void manageFragmentParameters(String[] parts) {
	}

	public void setMenuManager(RefVaadinMenuManager rootWindow) {
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.vaadin.terminal.ParameterHandler#handleParameters(java.util.Map)
	 */
	public void handleParameters(Map<String, String[]> parameters) {
		// TODO Auto-generated method stub

	}
}
