/**
 * 
 */
package it.infn.chaos.mds;

import it.infn.chaos.mds.process.ManageDeviceProcess;
import it.infn.chaos.mds.process.ManageServerProcess;
import it.infn.chaos.mds.process.ManageUnitServerProcess;

import java.util.Map;
import java.util.Observable;

import org.ref.server.webapp.RefVaadinApplicationController;
import org.ref.server.webapp.RefVaadinMenuManager;
import org.ref.server.webapp.RefVaadinApplicationController.OpenViewIn;

import com.vaadin.ui.Component;

/**
 * @author bisegni
 * 
 */
@SuppressWarnings("serial")
public class NewUSCUAssociationController extends RefVaadinApplicationController {
	private ManageDeviceProcess		mdp		= null;
	private ManageServerProcess		msp		= null;
	private ManageUnitServerProcess	musp	= null;

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
		//openViewByKeyAndClass("VISTA", NewUSCUAssociationView.class);
		openViewByKeyAndClass("NEW_ASSOC_US_CU", NewUSCUAssociationView.class);

	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see org.ref.server.webapp.RefViewAggregator#updateMainView(java.lang.String, org.ref.server.webapp.RefVaadinApplicationController, com.vaadin.ui.Component)
	 */
	@Override
	public void updateMainView(String key, RefVaadinApplicationController controller, Component applicationViewComponent) {
		// TODO Auto-generated method stub

	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.vaadin.terminal.ParameterHandler#handleParameters(java.util.Map)
	 */
	@Override
	public void handleParameters(Map<String, String[]> parameters) {
		// TODO Auto-generated method stub

	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see org.ref.server.webapp.RefVaadinApplicationController#updateMainView(org.ref.server.webapp.RefVaadinApplicationController, com.vaadin.ui.Component)
	 */
	@Override
	public void updateMainView(RefVaadinApplicationController controller, Component applicationViewComponent) {
		// TODO Auto-generated method stub

	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see org.ref.server.webapp.RefVaadinApplicationController#update(java.util.Observable, java.lang.Object)
	 */
	@Override
	public void update(Observable source, Object sourceData) {
		// TODO Auto-generated method stub

	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see org.ref.server.webapp.RefVaadinApplicationController#manageFragmentParameters(java.lang.String[])
	 */
	@Override
	protected void manageFragmentParameters(String[] parts) {
		// TODO Auto-generated method stub

	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see org.ref.server.webapp.RefVaadinApplicationController#setMenuManager(org.ref.server.webapp.RefVaadinMenuManager)
	 */
	@Override
	public void setMenuManager(RefVaadinMenuManager rootWindow) {
		// TODO Auto-generated method stub

	}

}
