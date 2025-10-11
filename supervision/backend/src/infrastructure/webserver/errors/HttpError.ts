import { HttpStatusEnum } from "../enum/HttpStatusEnum";


export class HttpError extends Error {
	public statusCode: number;
	public data?: any;

	constructor(statusCode: number, message: string, data?: any) {
		super(message);
		this.statusCode = statusCode;
		this.data = data;
		Object.setPrototypeOf(this, new.target.prototype);
	}
}

export class NotFound extends HttpError {
	constructor(message = "Not found", data?: any) {
		super(HttpStatusEnum.NOT_FOUND, message, data);
	}
}

export class BadRequest extends HttpError {
	constructor(message = "Bad request", data?: any) {
		super(HttpStatusEnum.BAD_REQUEST, message, data);
	}
}

export class Unauthorized extends HttpError {
	constructor(message = "Unauthorized", data?: any) {
		super(HttpStatusEnum.UNAUTHORIZED, message, data);
	}
}

export class Forbidden extends HttpError {
	constructor(message = "Forbidden", data?: any) {
		super(HttpStatusEnum.FORBIDDEN, message, data);
	}
}

export class InternalServerError extends HttpError {
	constructor(message = "Internal server error", data?: any) {
		super(HttpStatusEnum.INTERNAL_SERVER_ERROR, message, data);
	}
}

export class NotImplemented extends HttpError {
	constructor(message = "Not implemented") {
		super(HttpStatusEnum.NOT_IMPLEMENTED, message);
	}
}
