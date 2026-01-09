export class UserIdentity {
	constructor(
		public username: string,
	) {}

	/**
	 * Transforme le payload JWT en instance de UserIdentity
	 */
	static fromPayload(payload: any): UserIdentity {
		return new UserIdentity(
			payload.username
		);
	}
}
