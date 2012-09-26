/**
 * 
 */
package it.infn.chaos.chaos.common.rpc.msgpack;

import it.infn.chaos.chaos.common.*;
import it.infn.chaos.chaos.common.exception.ChaosException;
import it.infn.chaos.chaos.common.rpc.*;

import org.bson.BasicBSONDecoder;
import org.bson.BasicBSONEncoder;
import org.bson.BasicBSONObject;
import org.msgpack.rpc.Request;
import org.msgpack.rpc.Server;
import org.msgpack.type.RawValue;
import org.msgpack.type.ValueFactory;

/**
 * @author bisegni
 */
public class MSGPackRPCServer extends RPCServer {
	private BasicBSONDecoder		bDecoder			= new BasicBSONDecoder();
	private ActionHandlerExecutor	handlerExecution	= null;
	private Server					s					= new Server();

	/**
	 * 
	 * @param threadNumber
	 */
	public MSGPackRPCServer(int threadNumber) {
		handlerExecution = new ActionHandlerExecutor(threadNumber);
	}

	/*
	 * (non-Javadoc)
	 * @see it.infn.chaos.mds.rpc.RPCServer#init()
	 */
	@Override
	public void init(int port) throws Throwable {
		s = new Server();
		s.listen(port);
	}

	/*
	 * (non-Javadoc)
	 * @see it.infn.chaos.mds.rpc.RPCServer#start()
	 */
	@Override
	public void start() throws Throwable {
		s.serve(this);
	}

	/*
	 * (non-Javadoc)
	 * @see it.infn.chaos.mds.rpc.RPCServer#deinit()
	 */
	@Override
	public void deinit() throws Throwable {
		s.close();
		handlerExecution.stop();
	}

	/**
	 * Chaos message
	 * 
	 * @param request
	 * @param msg
	 * @param a
	 */
	public void chaos_rpc(Request request, RawValue messageData) {
		BasicBSONObject bsonData = (BasicBSONObject) bDecoder.readObject(messageData.asRawValue().getByteArray());

		String domain = bsonData.getString(RPCConstants.CS_CMDM_ACTION_DOMAIN);
		String action = bsonData.getString(RPCConstants.CS_CMDM_ACTION_NAME);

		RPCActionHadler actionHandler;
		try {
			actionHandler = getHandlerForDomainAndAction(domain, action);
			if (actionHandler == null) {
				request.sendResult(getResponseRawType("No action handler", "MSGPackRPCServer", 1));
				return;
			}
			handlerExecution.submitAction(actionHandler, bsonData);
			request.sendResult(getResponseRawType(null, null, 0));
		} catch (ChaosException e) {
			request.sendResult(getResponseRawType(e.getDomain(), e.getMessage(), e.getCode()));
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
	}

	/**
	 * @param submissionResult
	 * @param errorDomain
	 * @param errorMsg
	 * @param errCode
	 * @return
	 */
	private RawValue getResponseRawType(String errorDomain, String errorMsg, int errCode) {
		BasicBSONObject bbObj = new BasicBSONObject();
		BasicBSONEncoder enc = new BasicBSONEncoder();
		RPCUtils.addRefExceptionElementToBson(bbObj, errorDomain, errorMsg, errCode);
		return ValueFactory.createRawValue(enc.encode(bbObj));
	}
}
