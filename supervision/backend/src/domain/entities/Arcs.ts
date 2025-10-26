import {
  Column,
  Entity,
  Index,
  JoinColumn,
  ManyToOne,
  PrimaryGeneratedColumn,
} from "typeorm";
import { Nodes } from "./Nodes";

@Index("origin_node", ["originNode"], {})
@Index("destination_node", ["destinationNode"], {})
@Entity("ARCS", { schema: "supervision" })
export class Arcs {
  @PrimaryGeneratedColumn({ type: "int", name: "id" })
  public id!: number;

  @Column("int", { name: "weight", nullable: true })
  public weight!: number | null;

  @Column("int", { name: "origin_node", nullable: true })
  public originNode!: number | null;

  @Column("int", { name: "destination_node", nullable: true })
  public destinationNode!: number | null;

  @ManyToOne(() => Nodes, (nodes) => nodes.arcs, {
    onDelete: "NO ACTION",
    onUpdate: "NO ACTION",
  })
  @JoinColumn([{ name: "origin_node", referencedColumnName: "id" }])
  public originNode2!: Nodes;

  @ManyToOne(() => Nodes, (nodes) => nodes.arcs2, {
    onDelete: "NO ACTION",
    onUpdate: "NO ACTION",
  })
  @JoinColumn([{ name: "destination_node", referencedColumnName: "id" }])
  public destinationNode2!: Nodes;

  public constructor(init?: Partial<Arcs>) {
    Object.assign(this, init);
  }
}
