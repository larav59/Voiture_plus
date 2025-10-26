import {
  Column,
  Entity,
  Index,
  JoinColumn,
  ManyToOne,
  PrimaryGeneratedColumn,
} from "typeorm";
import { Vehicles } from "./Vehicles";

@Index("vehicle_id", ["vehicleId"], {})
@Entity("STATES", { schema: "supervision" })
export class States {
  @PrimaryGeneratedColumn({ type: "int", name: "id" })
  public id!: number;

  @Column("float", { name: "speed", nullable: true, precision: 12 })
  public speed!: number | null;

  @Column("float", { name: "angle", nullable: true, precision: 12 })
  public angle!: number | null;

  @Column("float", { name: "position_x", nullable: true, precision: 12 })
  public positionX!: number | null;

  @Column("float", { name: "position_y", nullable: true, precision: 12 })
  public positionY!: number | null;

  @Column("timestamp", { name: "occured_at", nullable: true })
  public occuredAt!: Date | null;

  @Column("int", { name: "vehicle_id", nullable: true })
  public vehicleId!: number | null;

  @ManyToOne(() => Vehicles, (vehicles) => vehicles.states, {
    onDelete: "NO ACTION",
    onUpdate: "NO ACTION",
  })
  @JoinColumn([{ name: "vehicle_id", referencedColumnName: "id" }])
  public vehicle!: Vehicles;

  public constructor(init?: Partial<States>) {
    Object.assign(this, init);
  }
}
