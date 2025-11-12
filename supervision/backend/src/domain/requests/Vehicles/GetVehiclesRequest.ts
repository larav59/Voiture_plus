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
		const queryString = req.query;
		queryString.id = queryString.id !== undefined ? parseInt(queryString.id) : undefined;
		queryString.name = queryString.name !== undefined ? String(queryString.name) : undefined;

		return new GetVehiclesRequest({
			id: queryString.id,
			name: queryString.name,
		});
	}
}