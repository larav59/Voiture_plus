import { Validator } from "../../../infrastructure/validator/Validator";
import { BaseRequest } from "../BaseRequest";

export class GetVehiclesRequest extends BaseRequest<GetVehiclesRequest> {
	vehicleId : number | null;
	name : string | null;

	constructor(fields?: Partial<GetVehiclesRequest>) {
		super();
		this.vehicleId = fields?.vehicleId ?? null;
		this.name = fields?.name ?? null;
	}

	// Méthode pour la validation
	public validate(): Validator<GetVehiclesRequest> {
		const validator = new Validator<GetVehiclesRequest>(this);
		validator.field("vehicleId").isNumber().greaterThan(0);
		return validator;
	}

	static fromRequest(req: any): GetVehiclesRequest {
		const query = req.query;
		return new GetVehiclesRequest({
			vehicleId: query.vehicleId ? Number(query.vehicleId) : null,
			name: query.name,
		});
	}
}