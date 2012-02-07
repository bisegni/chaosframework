package it.infn.chaos.mds.rpcaction;

import java.util.Date;

import org.bson.BasicBSONObject;
import org.ref.common.exception.RefException;

import it.infn.chaos.mds.rpc.server.RPCActionHadler;

public class PerformanceTest extends RPCActionHadler {
	private static final String	SYSTEM	= "system";
	private static final String	PERF	= "perf";

	@Override
	public void intiHanlder() throws RefException {
		addDomainAction(SYSTEM, PERF);

	}

	@Override
	public BasicBSONObject handleAction(String domain, String action, BasicBSONObject actionData) throws Throwable {
		if (domain.equals(SYSTEM) && action.equals(PERF)) {
			return new BasicBSONObject().append("perf", new Date());
		}
		return null;
	}

}
