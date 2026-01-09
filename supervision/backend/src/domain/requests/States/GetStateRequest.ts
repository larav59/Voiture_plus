import { Validator } from "../../../infrastructure/validator/Validator";
import { BaseRequest } from "../BaseRequest";

export class GetStateRequest extends BaseRequest<GetStateRequest> {
	public vehicleId: number | null;
	public occuredBefore: Date | null;
	public occuredAfter: Date | null;
	

	constructor(fields?: Partial<GetStateRequest>) {
		super();
		this.vehicleId = fields?.vehicleId ?? null;
		this.occuredBefore = fields?.occuredBefore ?? null;
		this.occuredAfter = fields?.occuredAfter ?? null;
	}

	// Méthode pour la validation
	public validate(): Validator<GetStateRequest> {
		const validator = new Validator<GetStateRequest>(this);
		validator.field("vehicleId").isNumber().greaterThan(0);
		validator.field("occuredBefore").isDate();
		validator.field("occuredAfter").isDate();
		return validator;
	}

	static fromRequest(req: any): GetStateRequest {
		const query = req.query;
		return new GetStateRequest({
			vehicleId: Number(query.vehicleId) ? Number(query.vehicleId) : null,
			occuredBefore: query.occuredBefore ? new Date(query.occuredBefore) : null,
			occuredAfter: query.occuredAfter ? new Date(query.occuredAfter) : null
		});
	}
}