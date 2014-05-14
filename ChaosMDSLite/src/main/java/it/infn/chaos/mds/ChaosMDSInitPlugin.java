/**
 * 
 */
package it.infn.chaos.mds;

import it.infn.chaos.mds.rpc.server.JMQRPCClient;
import it.infn.chaos.mds.rpc.server.JMQRPCServer;
import it.infn.chaos.mds.rpc.server.MSGPackRPCClient;
import it.infn.chaos.mds.rpc.server.MSGPackRPCServer;
import it.infn.chaos.mds.rpcaction.CUQueryHandler;
import it.infn.chaos.mds.rpcaction.DeviceQueyHandler;
import it.infn.chaos.mds.rpcaction.PerformanceTest;

import org.ref.server.configuration.REFServerConfiguration;
import org.ref.server.plugins.REFDeinitPlugin;
import org.ref.server.plugins.REFInitPlugin;

/**
 * Inizializzaiozne del lato serve di GOVA
 * 
 * @author bisegni
 */
public class ChaosMDSInitPlugin implements REFInitPlugin, REFDeinitPlugin {

	/*
	 * (non-Javadoc)
	 * 
	 * @see org.ref.server.slfservletadaptor.RTTFInitPlugin#custoInit()
	 */
	public void customInit() {
		try {
			String port = REFServerConfiguration.getInstance().getStringParamByValue("mds.port");
			int intPort = 5000;
			try {
				intPort = Integer.parseInt(port);
			} catch (Exception e) {
			}
			SingletonServices.getInstance().setMdsRpcServer(new JMQRPCServer(1));
			SingletonServices.getInstance().setMdsRpcClient(new JMQRPCClient(1));
			// SingletonServices.getInstance().setMdsRpcServer(new MSGPackRPCServer(8));
			// SingletonServices.getInstance().setMdsRpcClient(new MSGPackRPCClient());
			SingletonServices.getInstance().getMdsRpcClient().init(0);
			SingletonServices.getInstance().getMdsRpcClient().start();
			SingletonServices.getInstance().getMdsRpcServer().init(intPort);
			SingletonServices.getInstance().getMdsRpcServer().start();

			SingletonServices.getInstance().addHandler(CUQueryHandler.class);
			SingletonServices.getInstance().addHandler(DeviceQueyHandler.class);
			SingletonServices.getInstance().addHandler(PerformanceTest.class);
		} catch (Throwable e) {
			e.printStackTrace();
		}

	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see org.ref.server.plugins.REFDeinitPlugin#customDeinit()
	 */
	public void customDeinit() {
		try {
			SingletonServices.getInstance().getMdsRpcServer().deinit();
			SingletonServices.getInstance().getMdsRpcClient().deinit();
		} catch (Throwable e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

	}

}
