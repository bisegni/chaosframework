/**
 * 
 */
package it.infn.chaos.mds;

import it.infn.chaos.mds.rpc.server.JMQRPCClient;
import it.infn.chaos.mds.rpc.server.JMQRPCServer;
import it.infn.chaos.mds.rpc.server.TCPRpcClient;
import it.infn.chaos.mds.rpc.server.TCPRpcServer;
import it.infn.chaos.mds.rpcaction.CUQueryHandler;
import it.infn.chaos.mds.rpcaction.DeviceQueyHandler;
import it.infn.chaos.mds.rpcaction.PerformanceTest;
import it.infn.chaos.mds.slowexecution.SlowExecution;
import it.infn.chaos.mds.slowexecution.UnitServerACK;

import java.util.Arrays;
import java.util.StringTokenizer;
import java.util.Vector;

import org.ref.server.configuration.REFServerConfiguration;
import org.ref.server.plugins.REFDeinitPlugin;
import org.ref.server.plugins.REFInitPlugin;

import com.mongodb.MongoClient;
import com.mongodb.MongoCredential;
import com.mongodb.ServerAddress;
import com.mongodb.WriteConcern;

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
			String mongodbURL = REFServerConfiguration.getInstance().getStringParamByValue("mongodb.url");
			String user = REFServerConfiguration.getInstance().getStringParamByValue("mongodb.user");
			String pwd = REFServerConfiguration.getInstance().getStringParamByValue("mongodb.pwd");
			String db = REFServerConfiguration.getInstance().getStringParamByValue("mongodb.db");
			String rpcImpl = REFServerConfiguration.getInstance().getStringParamByValue("chaos.rpc.impl");

			if (mongodbURL != null) {
				StringTokenizer tokenizer = new StringTokenizer(mongodbURL, ",");
				Vector<ServerAddress> servers = new Vector<ServerAddress>();
				while (tokenizer.hasMoreTokens()) {
					String databaseUrl = tokenizer.nextToken();
					servers.add(new ServerAddress(databaseUrl));
				}
				if (servers.size() > 0) {
					if (user != null && pwd != null && db != null) {
						SingletonServices.getInstance().setMongoClient(new MongoClient(servers, Arrays.asList(MongoCredential.createMongoCRCredential(user, db, pwd.toCharArray()))));
					} else {
						SingletonServices.getInstance().setMongoClient(new MongoClient(servers));
					}
					SingletonServices.getInstance().setMongoDB(SingletonServices.getInstance().getMongoClient().getDB(db));
					SingletonServices.getInstance().getMongoClient().setWriteConcern(WriteConcern.JOURNALED);
				}
			}
			int intPort = 5000;
			try {
				intPort = Integer.parseInt(port);
			} catch (Exception e) {
			}
			if (rpcImpl.equals("TCPRpc")) {
				SingletonServices.getInstance().setMdsRpcServer(new TCPRpcServer());
				SingletonServices.getInstance().setMdsRpcClient(new TCPRpcClient());
			} else if (rpcImpl.equals("JMQRPC")) {
				SingletonServices.getInstance().setMdsRpcServer(new JMQRPCServer(1));
				SingletonServices.getInstance().setMdsRpcClient(new JMQRPCClient(1));
			}
			if (SingletonServices.getInstance().getMdsRpcClient() != null && SingletonServices.getInstance().getMdsRpcServer() != null) {
				SingletonServices.getInstance().getMdsRpcClient().init(0);
				SingletonServices.getInstance().getMdsRpcClient().start();
				SingletonServices.getInstance().getMdsRpcServer().init(intPort);
				SingletonServices.getInstance().getMdsRpcServer().start();
			}
			SingletonServices.getInstance().addHandler(CUQueryHandler.class);
			SingletonServices.getInstance().addHandler(DeviceQueyHandler.class);
			SingletonServices.getInstance().addHandler(PerformanceTest.class);
			
			//allocate new slow execution queue
			SingletonServices.getInstance().setSlowExecution(new SlowExecution(20));
			SingletonServices.getInstance().getSlowExecution().registerSlowControlJob(UnitServerACK.class);
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
