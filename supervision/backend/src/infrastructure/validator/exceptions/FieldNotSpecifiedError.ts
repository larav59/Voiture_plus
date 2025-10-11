
export class FieldNotSpecifiedError extends Error {
	constructor() {
		super("Field not specified: Make sure to call .on() before calling validation methods");
		this.name = "FieldNotSpecifiedError";
	}
}