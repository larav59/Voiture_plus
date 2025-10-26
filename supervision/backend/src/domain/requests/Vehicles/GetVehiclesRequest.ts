import { Validator } from "../../../infrastructure/validator/Validator";
import { BaseRequest } from "../BaseRequest";

export class GetVehiclesRequest extends BaseRequest<GetVehiclesRequest> {
	id : number | null;
	name : string | null;

	constructor(fields?: Partial<GetVehiclesRequest>) {
		super();
		this.id = fields?.id ?? null;
		this.name = fields?.name ?? null;
	}

	// Méthode pour la validation
	public validate(): Validator<GetVehiclesRequest> {
		const validator = new Validator<GetVehiclesRequest>(this);
		validator.field("id").isNumber().greaterThan(0);
		return validator;
	}

	static fromRequest(req: any): GetVehiclesRequest {
		const body = req.body
		body.id = body.id !== undefined ? parseInt(body.id) : undefined;
		body.name = body.name !== undefined ? String(body.name) : undefined;

		return new GetVehiclesRequest({
			id: body.id,
			name: body.name,
		});
	}
}