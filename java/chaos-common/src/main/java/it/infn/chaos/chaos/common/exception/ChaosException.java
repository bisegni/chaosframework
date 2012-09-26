package it.infn.chaos.chaos.common.exception;

public class ChaosException extends Throwable {

	private static final long	serialVersionUID	= 1145004830415391979L;
	private int					code				= 0;
	private String				domain				= "";

	public ChaosException(String errorMessage, int errorCode, String errorDomain) {
		super(errorMessage);
		this.code = errorCode;
		this.domain = errorDomain;
	}

	/**
	 * @return the errorCode
	 */
	public int getCode() {
		return code;
	}

	/**
	 * @return the errorDomain
	 */
	public String getDomain() {
		return domain;
	}

}
