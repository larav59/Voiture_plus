export class UserIdentity {
	constructor(
		public id: string,
	) {}

	/**
	 * Transforme le payload JWT en instance de UserIdentity
	 */
	static fromPayload(payload: any): UserIdentity {
		return new UserIdentity(
			payload.uid
		);
	}
}
