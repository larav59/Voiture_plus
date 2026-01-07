import { Validator } from "../../../infrastructure/validator/Validator";
import { BaseRequest } from "../BaseRequest";

export class CreateNodeRequest extends BaseRequest<CreateNodeRequest> {
	name : string
	type : number
	positionX : number
	positionY : number
	offsetX : number
	offsetY : number
	isPointOfInterest : boolean

	constructor(fields?: Partial<CreateNodeRequest>) {
		super();
		this.name = fields?.name!;
		this.type = fields?.type!;
		this.positionX = fields?.positionX!;
		this.positionY = fields?.positionY!;
		this.offsetX = fields?.offsetX!;
		this.offsetY = fields?.offsetY!;
		this.isPointOfInterest = fields?.isPointOfInterest!;
	}

	// Méthode pour la validation
	public validate(): Validator<CreateNodeRequest> {
		const validator = new Validator<CreateNodeRequest>(this);
		validator.field("type").isNumber().greaterThan(0);
		validator.field("positionX").isRequired().isNumber();
		validator.field("positionY").isRequired().isNumber();
		validator.field("offsetX").isRequired().isNumber();
		validator.field("offsetY").isRequired().isNumber()
		validator.field("name").isRequired().minLength(1).maxLength(255);
		validator.field("isPointOfInterest").isRequired().isBoolean();
		return validator;
	}

	static fromRequest(req: any): CreateNodeRequest {
		const body = req.body;
		return new CreateNodeRequest({
			name: body.name,
			type: body.type,
			positionX : body.positionX,
			positionY : body.positionY,
			offsetX : body.offsetX,
			offsetY : body.offsetY,
			isPointOfInterest: body.isPointOfInterest,
		});
	}
}