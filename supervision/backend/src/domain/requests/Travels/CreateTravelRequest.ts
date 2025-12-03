import { Validator } from "../../../infrastructure/validator/Validator";
import { BaseRequest } from "../BaseRequest";

export class CreateTravelRequest extends BaseRequest<CreateTravelRequest> {
	vehicle : number | null;
	nodes : any | null;
	
	constructor(fields?: Partial<CreateTravelRequest>) {
		super();
		this.nodes = fields?.nodes ?? null;
		this.vehicle = fields?.vehicle ?? null;
	}

	// Méthode pour la validation
	public validate(): Validator<CreateTravelRequest> {
		const validator = new Validator<CreateTravelRequest>(this);
		validator.field("vehicle").isRequired();
		validator.field("nodes").isRequired();
		return validator;
	}

	static fromRequest(req: any): CreateTravelRequest {
		const body = req.body;
		return new CreateTravelRequest({
			nodes: body.nodes,
			vehicle: body.vehicle
		});
	}
}