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
		const query = req.query;
		return new GetVehiclesRequest({
			id: query.id,
			name: query.name,
		});
	}
}