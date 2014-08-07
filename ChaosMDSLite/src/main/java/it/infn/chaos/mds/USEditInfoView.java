/**
 * 
 */
package it.infn.chaos.mds;

import it.infn.chaos.mds.business.UnitServer;
import it.infn.chaos.mds.secutiry.RSAKeys;

import java.security.KeyPair;
import java.security.KeyPairGenerator;
import java.security.NoSuchAlgorithmException;
import java.security.NoSuchProviderException;
import java.security.PrivateKey;
import java.security.PublicKey;
import java.security.SecureRandom;
import java.security.Security;
import java.util.Observable;

import org.bouncycastle.jce.provider.BouncyCastleProvider;
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
public class USEditInfoView extends RefVaadinBasePanel implements com.vaadin.data.Container.PropertySetChangeListener, ItemClickListener {
	private static final long	serialVersionUID			= -841710385419733478L;
	public static final String	EVENT_SAVE					= "USEditInfoView_EVENT_SAVE";
	public static final String	EVENT_CANCELL				= "USEditInfoView_EVENT_CANCELL";
	public static final String	EVENT_DELETE_SECURITY_KEY	= "USEditInfoView_EVENT_DELETE_SECURITY_KEY";
	public static final String	EVENT_SET_UNIT_SERVER		= "USEditInfoView_EVENT_SET_UNIT_SERVER";
	private USEditInfoVaadin	impl						= new USEditInfoVaadin();
	private UnitServer			workingUnitServer			= null;

	/*
	 * (non-Javadoc)
	 * 
	 * @see org.ref.server.webapp.RefVaadinBasePanel#initGui()
	 */
	@SuppressWarnings("serial")
	@Override
	public void initGui() {
		addComponent(impl);
		impl.getTextAreaPublicKey().setReadOnly(true);
		impl.getButtonCancell().addListener(new ClickListener() {
			public void buttonClick(ClickEvent event) {
				notifyEventoToControllerWithData(USEditInfoView.EVENT_CANCELL, event.getSource(), null);
			}
		});
		impl.getButtonSaveInfo().addListener(new ClickListener() {
			public void buttonClick(ClickEvent event) {
				// get new alias value
				workingUnitServer.setAlias(impl.getTextFieldUnitServerAlias().getValue().toString());

				// send save event
				notifyEventoToControllerWithData(USEditInfoView.EVENT_SAVE, event.getSource(), workingUnitServer);
			}
		});

		impl.getButtonGenerateSecurityKeys().addListener(new ClickListener() {
			public void buttonClick(ClickEvent event) {

				try {
					RSAKeys keys = new RSAKeys();
					keys.generate();

					workingUnitServer.setPublic_key(keys.getPublicKeyAsB64String());
					workingUnitServer.setPrivate_key(keys.getPrivateKeyAsB64String());

					impl.getTextAreaPublicKey().setReadOnly(false);
					impl.getTextAreaPublicKey().setValue(workingUnitServer.getPublic_key());
					impl.getTextAreaPublicKey().setReadOnly(true);
				} catch (NoSuchAlgorithmException e) {
					getWindow().showNotification("Error creating id security keys");
				} catch (NoSuchProviderException e) {
					getWindow().showNotification("Error creating id security keys");
				}
			}
		});

		impl.getButtonRemoveSecurityKeys().addListener(new ClickListener() {
			public void buttonClick(ClickEvent event) {
				workingUnitServer.setPublic_key("");
				workingUnitServer.setPrivate_key("");
				impl.getTextAreaPublicKey().setReadOnly(false);
				impl.getTextAreaPublicKey().setValue(workingUnitServer.getPublic_key());
				impl.getTextAreaPublicKey().setReadOnly(true);
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
			if (viewEvent.getEventKind().equals(USEditInfoView.EVENT_SET_UNIT_SERVER)) {
				workingUnitServer = (UnitServer) viewEvent.getEventData();
				impl.getTextFieldUnitServerAlias().setValue(workingUnitServer.getAlias());

				impl.getTextAreaPublicKey().setReadOnly(false);
				impl.getTextAreaPublicKey().setValue(workingUnitServer.getPublic_key());
				impl.getTextAreaPublicKey().setReadOnly(true);
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
